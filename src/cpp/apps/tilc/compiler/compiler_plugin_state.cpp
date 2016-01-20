#include "compiler_plugin_state.h"
#include "compiler_comment_state.h"
#include "compiler_var_state.h"

CompilerPluginState::CompilerPluginState(CompilerState* parent)
: CompilerState("plugin", parent), m_state(Name)
{
    m_useTokens.push_back('\r');
    m_useTokens.push_back('\n');
    m_useTokens.push_back('{');
    m_useTokens.push_back('}');
    m_useTokens.push_back(' ');
    m_useTokens.push_back(';');
    m_useTokens.push_back('\t');
}

CompilerPluginState::~CompilerPluginState(){}
CompilerState::StateStatus CompilerPluginState::CheckIsUseWord(const std::string& word)
{
    if(word == "//" || word == "/*")
    {
        return CompilerState::StateApply;
    }
    
    return CompilerState::StateContinue;
}

CompilerState* CompilerPluginState::GetNextState(const std::string& word, char token)
{
    if(word.empty() && token == 0)
        return this;
    else if(word == "//" || word == "/*")
    {
        m_childState = new CompilerCommentState(this, word);
        return m_childState;
    }
    else if(token == ' ' || token == '\t' ||
           token == '\r' || token == '\n')
    {
        if(word.empty())
            return this;
        else if(m_state == Name)
        {
            m_state = PreBody;
            m_pluginName = word;
            return this;
        }
        else if(m_state == PreBody)
            return 0;
        else if(m_state == Body &&
                (word == "int" ||
                 word == "bool" ||
                 word == "string" ||
                 word == "float" ||
                 word == "void"))
        {
            m_childState = new CompilerVarState(this, word);
            return m_childState;
        }
    }
    else if(token == '{')
    {
        if (m_state == Name && word.empty() ||
            m_state == PreBody && !word.empty())
            return 0;
        else if(m_state == Name)
        {
            m_state = Body;
            m_pluginName = word;
            return this;
        }
        else if(m_state == PreBody)
        {
            m_state = Body;
            return this;
        }
    }
    else if(token == ';' && m_state == Body && word.empty())
        return this;
    else if(token == '}' && m_state == Body)
    {
        return m_parentState;
    }
    
    return 0;
}
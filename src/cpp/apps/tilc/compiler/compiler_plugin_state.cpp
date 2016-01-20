#include "compiler_plugin_state.h"
#include "compiler_comment_state.h"
#include "compiler_var_state.h"

CompilerPluginState::CompilerPluginState(CompilerState* parent)
: CompilerState("plugin", parent), m_state(Name)
{
    m_errorString.push_back("(");
    m_errorString.push_back(")");
    m_errorString.push_back(",");
    
    m_ignoreString.push_back("\r");
    m_ignoreString.push_back("\n");
}

CompilerPluginState::~CompilerPluginState(){}

CompilerState::StateStatus CompilerPluginState::CheckIsNextState(char token)
{
    if(token == '{')
    {
        if(m_state == Name ||
           m_state == PreBody && !m_pluginName.empty())
        {
            m_state = PreBody;
            return CompilerState::StateApply;
        }
        else if(m_state == PreBody)
        {
            return CompilerState::StateError;
        }
    }
    else if(token == '}' && m_state == Body)
    {
        return CompilerState::StateApply;
    }
    else if(token == ' ' || token == '\t')
    {
        if(m_state == PreBody)
        {
            return CompilerState::StateIgnore;
        }
        
        if(m_state == Body)
        {
            if(m_checkWord.empty())
            {
                return CompilerState::StateIgnore;
            }
        }
        
        return CompilerState::StateApply;
    }
    
    return CompilerState::StateContinue;
}

CompilerState::StateStatus CompilerPluginState::CheckIsNextState(const std::string& word)
{
    m_checkWord = word;
    if(word == "//" || word == "/*")
    {
        return CompilerState::StateApply;
    }
    
    return CompilerState::StateContinue;
}

CompilerState* CompilerPluginState::GetNextState(const std::string& word)
{
    CompilerState* state = 0;
    if(m_state == Name)
    {
        if(!word.empty())
        {
            m_state = PreBody;
            m_pluginName = word;
        }
        state = this;
    }
    else if(m_state == PreBody)
    {
        if (m_pluginName.empty() && word.empty() ||
            !m_pluginName.empty() && !word.empty())
            return 0;
        
        m_state = Body;
        if(m_pluginName.empty())
        {
            m_pluginName = word;
        }
        state = this;
    }
    
    if(m_checkWord == "//" || m_checkWord == "/*")
    {
        m_childState = new CompilerCommentState(this, m_checkWord);
        m_checkWord.clear();
        return m_childState;
    }
    else if(m_state == Body && 
           (m_checkWord == "int" ||
            m_checkWord == "bool" ||
            m_checkWord == "float" ||
            m_checkWord == "void" ||
            m_checkWord == "string"))
    {
        m_checkWord.clear();
        m_childState = new CompilerVarState(this, m_checkWord);
        return m_childState;
    }
    else if(m_state == Body &&
            m_checkWord == "module")
    {
    }
    
    return state;
}
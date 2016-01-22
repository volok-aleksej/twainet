#include "compiler_plugin_app_state.h"
#include "compiler_comment_state.h"
#include "compiler_var_state.h"
#include "compiler_module_state.h"
#include "til_compiler.h"

CompilerPluginAppState::CompilerPluginAppState(const std::string& name, CompilerState* parent, ICompilerEvent* event)
: CompilerState(name, parent, event), m_state(Name)
{
    m_useTokens.push_back('\r');
    m_useTokens.push_back('\n');
    m_useTokens.push_back('{');
    m_useTokens.push_back('}');
    m_useTokens.push_back(' ');
    m_useTokens.push_back(';');
    m_useTokens.push_back('\t');
}

CompilerPluginAppState::~CompilerPluginAppState(){}
CompilerState::StateStatus CompilerPluginAppState::CheckIsUseWord(const std::string& word)
{
    if(word == "//" || word == "/*")
    {
        return CompilerState::StateApply;
    }
    
    return CompilerState::StateContinue;
}

CompilerState* CompilerPluginAppState::GetNextState(const std::string& word, char token)
{
    if(word.empty() && token == 0)
        return this;
    else if(word == "//" || word == "/*")
    {
        m_childState = new CompilerCommentState(this, m_event, word);
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
            m_name = word;
            return this;
        }
        else if(m_state == PreBody)
            return 0;
        else if(m_state == Body &&
                (word == "int" ||
                 word == "bool" ||
                 word == "string" ||
                 word == "float" ||
                 word == "void" ||
                 word == "short" ||
                 word == "char"))
        {
            m_childState = new CompilerVarState(this, m_event, word);
            return m_childState;
        }
        else if(m_state == Body &&
                word == "module")
        {
            m_childState = new CompilerModuleState(this, m_event);
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
            m_name = word;
        }
        else if(m_state == PreBody)
            m_state = Body;
        else
            return 0;
        
        OnBegin();
        return this;
    }
    else if(token == ';' && m_state == Body && word.empty())
        return this;
    else if(token == '}' && m_state == Body)
    {
        OnEnd();
        return m_parentState;
    }
    
    return 0;
}

void CompilerApplicationState::OnBegin()
{
    m_event->onApplicationBegin(m_name);
}

void CompilerPluginState::OnBegin()
{
    m_event->onPluginBegin(m_name);
}

void CompilerApplicationState::OnEnd()
{
    m_event->onApplicationEnd();
}

void CompilerPluginState::OnEnd()
{
    m_event->onPluginEnd();
}
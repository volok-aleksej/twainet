#include "compiler_root_state.h"
#include "compiler_comment_state.h"
#include "compiler_plugin_app_state.h"

CompilerRootState::CompilerRootState(ICompilerEvent* event)
: CompilerState("root", 0, event)
{
    m_useTokens.push_back('{');
    m_useTokens.push_back('(');
    m_useTokens.push_back(';');
    m_useTokens.push_back(' ');
    m_useTokens.push_back('\r');
    m_useTokens.push_back('\n');
    m_useTokens.push_back('\t');
}

CompilerRootState::~CompilerRootState(){}

CompilerState::StateStatus CompilerRootState::CheckIsUseWord(const std::string& word)
{
    if(word == "//" || word == "/*")
    {
        return CompilerState::StateApply;
    }
    
    return CompilerState::StateContinue;
}

CompilerState* CompilerRootState::GetNextState(const std::string& word, char token)
{
    if (word == "//" || word == "/*")
    {
        m_childState = new CompilerCommentState(this, m_event, word);
        return m_childState;
    }
    else if(token == '{')
    {
    }
    else if(token == '(')
    {
    }
    else if(word.empty())
    {
        return this;
    }    
    else if (word == "plugin")
    {
        m_childState = new CompilerPluginState(this, m_event);
        return m_childState;
    }    
    else if (word == "application")
    {
        m_childState = new CompilerApplicationState(this, m_event);
        return m_childState;
    }
    
    return 0;
}
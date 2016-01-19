#include "compiler_root_state.h"
#include "compiler_comment_state.h"
#include "compiler_plugin_state.h"

CompilerRootState::CompilerRootState()
: CompilerState("root", 0){}

CompilerRootState::~CompilerRootState(){}

CompilerState::StateStatus CompilerRootState::CheckIsNextState(char token)
{
    if(token == ' ')
    {
        return CompilerState::StateApply;
    }
    
    return CompilerState::StateContinue;
}

CompilerState::StateStatus CompilerRootState::CheckIsNextState(const std::string& word)
{
    m_checkWord = word;
    if(word == "//" || word == "/*")
    {
        return CompilerState::StateApply;
    }
    
    return CompilerState::StateContinue;
}

CompilerState* CompilerRootState::GetNextState(const std::string& word)
{
    if ((m_checkWord == "//" ||
        m_checkWord == "/*") &&
        word.empty())
    {
        m_childState = new CompilerCommentState(this, m_checkWord);
    }
    
    if (m_checkWord == "plugin")
    {
        m_childState = new CompilerPluginState(this);
    }
    
    m_checkWord.clear();
    return m_childState;
}
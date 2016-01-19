#include "compiler_root_state.h"
#include "compiler_comment_state.h"
#include "compiler_plugin_state.h"

CompilerRootState::CompilerRootState()
: CompilerState("root", 0)
{
    m_errorString.push_back("{");
    m_errorString.push_back("}");
    m_errorString.push_back("(");
    m_errorString.push_back(")");
    m_errorString.push_back(",");
    
    m_ignoreString.push_back(";");
}

CompilerRootState::~CompilerRootState(){}

CompilerState::StateStatus CompilerRootState::CheckIsNextState(char token)
{
    if(token == ' ' ||
       token == '\t' ||
       token == '\n' ||
       token == '\r')
    {
        return CompilerState::StateApply;
    }
    
    return CompilerState::StateContinue;
}

CompilerState::StateStatus CompilerRootState::CheckIsNextState(const std::string& word)
{
    m_checkWord = word;
    if(word == "//" ||
       word == "/*")
    {
        return CompilerState::StateApply;
    }
    
    return CompilerState::StateContinue;
}

CompilerState* CompilerRootState::GetNextState(const std::string& word)
{
    std::string checkword = m_checkWord;
    m_checkWord.clear();
    if ((checkword == "//" ||
        checkword == "/*") &&
        word.empty())
    {
        m_childState = new CompilerCommentState(this, checkword);
        return m_childState;
    }
    
    if (checkword == "plugin" && checkword == word)
    {
        m_childState = new CompilerPluginState(this);
        return m_childState;
    }
  
    if (word.empty())
    {
        return this;
    }
    
    return 0;
}
#include "compiler_function_state.h"
#include "compiler_comment_state.h"

CompilerFunctionState::CompilerFunctionState(CompilerState* parent, const std::string& retVal, const std::string& funcName)
: CompilerState("function", parent)
{
}

CompilerFunctionState::~CompilerFunctionState(){}
    
CompilerState::StateStatus CompilerFunctionState::CheckIsUseWord(const std::string& word)
{
    if(word == "//" || word == "/*")
    {
        return CompilerState::StateApply;
    }
    
    return CompilerState::StateContinue;
}

CompilerState* CompilerFunctionState::GetNextState(const std::string& word, char token)
{
    if(word.empty() && token == 0)
        return this;
    else if(word == "//" || word == "/*")
    {
        m_childState = new CompilerCommentState(this, word);
        return m_childState;
    }
    
    return 0;
}
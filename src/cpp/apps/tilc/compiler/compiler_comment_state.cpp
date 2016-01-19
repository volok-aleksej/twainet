#include "compiler_comment_state.h"

CompilerCommentState::CompilerCommentState(CompilerState* parent, const std::string& commentWord)
: CompilerState("compiler", parent), m_commentWord(commentWord){}

CompilerCommentState::~CompilerCommentState(){}

CompilerState::StateStatus CompilerCommentState::CheckIsNextState(char token)
{
    if((token == '\r' || token == '\n') && m_commentWord == "//")
    {
        return StateApply;
    }

    return StateContinue;
}

CompilerState::StateStatus CompilerCommentState::CheckIsNextState(const std::string& word)
{
    if(word == "*/" && m_commentWord == "/*")
    {
        return StateApply;
    }
    return StateContinue;
}

CompilerState* CompilerCommentState::GetNextState(const std::string& word)
{
    return m_parentState;
}
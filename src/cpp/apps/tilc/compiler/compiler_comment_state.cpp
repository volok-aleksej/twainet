#include "compiler_comment_state.h"

CompilerCommentState::CompilerCommentState(CompilerState* parent, const std::string& commentWord)
: CompilerState("compiler", parent), m_commentWord(commentWord)
{
    m_useTokens.push_back('\r');
    m_useTokens.push_back('\n');
}

CompilerCommentState::~CompilerCommentState(){}

CompilerState::StateStatus CompilerCommentState::CheckIsUseWord(const std::string& word)
{
    if(word == "*/" && m_commentWord == "/*")
    {
        return StateApply;
    }
    return StateContinue;
}

CompilerState* CompilerCommentState::GetNextState(const std::string& word, char token)
{
    if(word == "*/" && token == 0 ||
       m_commentWord == "//" &&
       (token == '\r' || token == '\n'))
    {
        return m_parentState;
    }
    return this;
}
#include "compiler_comment_state.h"
#include "til_compiler.h"

CompilerCommentState::CompilerCommentState(CompilerState* parent, ICompilerEvent* event, const std::string& commentWord)
: CompilerState("compiler", parent, event), m_commentWord(commentWord)
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
        m_event->onComment(word);
        return m_parentState;
    }
    return this;
}
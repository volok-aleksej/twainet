#ifndef COMPILER_COMMENT_STATE_H
#define COMPILER_COMMENT_STATE_H

#include "compiler_state.h"

class CompilerCommentState : public CompilerState
{
public:
    CompilerCommentState(CompilerState* parent, const std::string& commentWord);
    virtual ~CompilerCommentState();
protected:
    virtual StateStatus CheckIsNextState(const std::string& word, char token);
    virtual CompilerState* GetNextState(const std::string& word);
private:
    std::string m_commentWord;
};

#endif/*COMPILER_COMMENT_STATE_H*/
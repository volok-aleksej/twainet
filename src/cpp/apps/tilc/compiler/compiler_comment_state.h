#ifndef COMPILER_COMMENT_STATE_H
#define COMPILER_COMMENT_STATE_H

#include "compiler_state.h"

class CompilerCommentState : public CompilerState
{
public:
    CompilerCommentState(CompilerState* parent, const std::string& commentWord);
    virtual ~CompilerCommentState();
protected:
    virtual StateStatus CheckIsUseWord(const std::string& word);
    virtual CompilerState* GetNextState(const std::string& word, char token);
private:
    std::string m_commentWord;
};

#endif/*COMPILER_COMMENT_STATE_H*/
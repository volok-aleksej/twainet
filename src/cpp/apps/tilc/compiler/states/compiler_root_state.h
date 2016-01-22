#ifndef COMPILER_ROOT_STATE_H
#define COMPILER_ROOT_STATE_H

#include "compiler_state.h"

class CompilerRootState : public CompilerState
{
public:
    CompilerRootState(ICompilerEvent* event);
    virtual ~CompilerRootState();
protected:
    virtual StateStatus CheckIsUseWord(const std::string& word);
    virtual CompilerState* GetNextState(const std::string& word, char token);
};

#endif/*COMPILER_ROOT_STATE_H*/
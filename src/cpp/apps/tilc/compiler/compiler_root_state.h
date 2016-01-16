#ifndef COMPILER_ROOT_STATE_H
#define COMPILER_ROOT_STATE_H

#include "compiler_state.h"

class CompilerRootState : public CompilerState
{
public:
    CompilerRootState();
    virtual ~CompilerRootState();
protected:
    virtual StateStatus CheckIsNextState(const std::string& word, char token);
    virtual CompilerState* GetNextState(const std::string& word);
private:
    std::string m_checkWord;
};

#endif/*COMPILER_ROOT_STATE_H*/
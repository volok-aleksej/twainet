#ifndef COMPILER_FUNCTION_STATE_H
#define COMPILER_FUNCTION_STATE_H

#include "compiler_state.h"
#include <string>

class CompilerFunctionState : public CompilerState
{
public:
    CompilerFunctionState(CompilerState* parent, const std::string& retVal, const std::string& funcName);
    virtual ~CompilerFunctionState();
protected:
    virtual StateStatus CheckIsNextState(char token);
    virtual StateStatus CheckIsNextState(const std::string& word);
    virtual CompilerState* GetNextState(const std::string& word);
};

#endif/*COMPILER_FUNCTION_STATE_H*/
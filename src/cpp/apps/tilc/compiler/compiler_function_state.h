#ifndef COMPILER_FUNCTION_STATE_H
#define COMPILER_FUNCTION_STATE_H

#include "compiler_state.h"
#include <string>

class CompilerFunctionState : public CompilerState
{
public:
    CompilerFunctionState(CompilerState* parent, ICompilerEvent* event, const std::string& retVal, const std::string& funcName);
    virtual ~CompilerFunctionState();
protected:
    virtual StateStatus CheckIsUseWord(const std::string& word);
    virtual CompilerState* GetNextState(const std::string& word, char token);
private:
    std::string m_funcName;
    std::string m_retVal;
    
};

#endif/*COMPILER_FUNCTION_STATE_H*/
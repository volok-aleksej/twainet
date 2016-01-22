#ifndef COMPILER_VAR_STATE_H
#define COMPILER_VAR_STATE_H

#include "compiler_state.h"
#include <string>

class CompilerVarState : public CompilerState
{
    enum State
    {
        Name,
        End
    };
public:
    CompilerVarState(CompilerState* parent, ICompilerEvent* event, const std::string& varType);
    virtual ~CompilerVarState();
protected:
    virtual StateStatus CheckIsUseWord(const std::string& word);
    virtual CompilerState* GetNextState(const std::string& word, char token);
private:
    std::string m_varType;
    std::string m_varName;
    State m_state;
};

#endif/*COMPILER_VAR_STATE_H*/
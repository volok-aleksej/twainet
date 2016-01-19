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
    CompilerVarState(CompilerState* parent, const std::string& varType);
    virtual ~CompilerVarState();
protected:
    virtual StateStatus CheckIsNextState(char token);
    virtual StateStatus CheckIsNextState(const std::string& word);
    virtual CompilerState* GetNextState(const std::string& word);
private:
    std::string m_varType;
    std::string m_varName;
    char m_lastSep;
    State m_state;
};

#endif/*COMPILER_VAR_STATE_H*/
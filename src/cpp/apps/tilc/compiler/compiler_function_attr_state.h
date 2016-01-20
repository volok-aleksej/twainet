#ifndef COMPILER_FUNCTION_ATTR_STATE_H
#define COMPILER_FUNCTION_ATTR_STATE_H

#include "compiler_state.h"

class CompilerFunctionAttrState : public CompilerState
{
    enum State
    {
        Name,
        End
    };
public:
    CompilerFunctionAttrState(CompilerState* parent, const std::string& type);
    virtual ~CompilerFunctionAttrState();
protected:
    virtual StateStatus CheckIsUseWord(const std::string& word);
    virtual CompilerState* GetNextState(const std::string& word, char token);
private:
    std::string m_varType;
    std::string m_varName;
    State m_state;
};

#endif/*COMPILER_FUNCTION_ATTR_STATE_H*/
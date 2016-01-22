#ifndef COMPILER_MODULE_STATE_H
#define COMPILER_MODULE_STATE_H

#include "compiler_state.h"

class CompilerModuleState : public CompilerState
{
    enum State
    {
        Name,
        PreBody,
        Body
    };
public:
    CompilerModuleState(CompilerState* parent, ICompilerEvent* event);
    virtual ~CompilerModuleState();
protected:
    virtual StateStatus CheckIsUseWord(const std::string& word);
    virtual CompilerState* GetNextState(const std::string& word, char token);
private:
    std::string m_moduleName;
    State m_state;
};

#endif/*COMPILER_MODULE_STATE_H*/
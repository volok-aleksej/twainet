#ifndef COMPILER_PLUGIN_STATE_H
#define COMPILER_PLUGIN_STATE_H

#include "compiler_state.h"

class CompilerPluginState : public CompilerState
{
    enum State
    {
        Name,
        PreBody,
        Body
    };
public:
    CompilerPluginState(CompilerState* parent);
    virtual ~CompilerPluginState();
protected:
    virtual StateStatus CheckIsUseWord(const std::string& word);
    virtual CompilerState* GetNextState(const std::string& word, char token);
private:
    std::string m_pluginName;
    State m_state;
};

#endif/*COMPILER_PLUGIN_STATE_H*/
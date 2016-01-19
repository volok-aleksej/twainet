#ifndef COMPILER_PLUGIN_STATE_H
#define COMPILER_PLUGIN_STATE_H

#include "compiler_state.h"

class CompilerPluginState : public CompilerState
{
public:
    CompilerPluginState(CompilerState* parent);
    virtual ~CompilerPluginState();
protected:
    virtual StateStatus CheckIsNextState(char token);
    virtual StateStatus CheckIsNextState(const std::string& word);
    virtual CompilerState* GetNextState(const std::string& word);
private:
    std::string m_pluginName;
};

#endif/*COMPILER_PLUGIN_STATE_H*/
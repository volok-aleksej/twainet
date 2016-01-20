#ifndef COMPILER_PLUGIN_STATE_H
#define COMPILER_PLUGIN_STATE_H

#include "compiler_state.h"

class CompilerPluginAppState : public CompilerState
{
    enum State
    {
        Name,
        PreBody,
        Body
    };
public:
    CompilerPluginAppState(const std::string& name, CompilerState* parent);
    virtual ~CompilerPluginAppState();
protected:
    virtual StateStatus CheckIsUseWord(const std::string& word);
    virtual CompilerState* GetNextState(const std::string& word, char token);
private:
    std::string m_name;
    State m_state;
};

class CompilerPluginState : public CompilerPluginAppState
{
public:
    CompilerPluginState(CompilerState* parent): CompilerPluginAppState("plugin", parent){}
    virtual ~CompilerPluginState(){}
};

class CompilerApplicationState : public CompilerPluginAppState
{
public:
    CompilerApplicationState(CompilerState* parent): CompilerPluginAppState("application", parent){}
    virtual ~CompilerApplicationState(){}
};
#endif/*COMPILER_PLUGIN_STATE_H*/
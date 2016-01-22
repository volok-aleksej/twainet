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
    CompilerPluginAppState(const std::string& name, CompilerState* parent, ICompilerEvent* event);
    virtual ~CompilerPluginAppState();
protected:
    virtual StateStatus CheckIsUseWord(const std::string& word);
    virtual CompilerState* GetNextState(const std::string& word, char token);
    virtual void OnBegin() = 0;
    virtual void OnEnd() = 0;
protected:
    std::string m_name;
private:
    State m_state;
};

class CompilerPluginState : public CompilerPluginAppState
{
public:
    CompilerPluginState(CompilerState* parent, ICompilerEvent* event): CompilerPluginAppState("plugin", parent, event){}
    virtual ~CompilerPluginState(){}
protected:
    virtual void OnBegin();
    virtual void OnEnd();
};

class CompilerApplicationState : public CompilerPluginAppState
{
public:
    CompilerApplicationState(CompilerState* parent, ICompilerEvent* event): CompilerPluginAppState("application", parent, event){}
    virtual ~CompilerApplicationState(){}
protected:
    virtual void OnBegin();
    virtual void OnEnd();
};
#endif/*COMPILER_PLUGIN_STATE_H*/
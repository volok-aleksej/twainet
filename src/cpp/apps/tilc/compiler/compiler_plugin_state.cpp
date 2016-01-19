#include "compiler_plugin_state.h"

CompilerPluginState::CompilerPluginState(CompilerState* parent)
: CompilerState("plugin", parent){}

CompilerPluginState::~CompilerPluginState(){}

CompilerState::StateStatus CompilerPluginState::CheckIsNextState(char token)
{
    if(token == '\r' || token == '\n')
    {
        return CompilerState::StateIgnore;
    }
    if(token == '{' )
    {
        return CompilerState::StateApply;
    }
    
    return CompilerState::StateContinue;
}

CompilerState::StateStatus CompilerPluginState::CheckIsNextState(const std::string& word)
{
    return CompilerState::StateContinue;
}

CompilerState* CompilerPluginState::GetNextState(const std::string& word)
{
    m_pluginName = word;
    return this;
}
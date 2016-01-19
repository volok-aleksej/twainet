#include "compiler_function_state.h"

CompilerFunctionState::CompilerFunctionState(CompilerState* parent, const std::string& retVal, const std::string& funcName)
: CompilerState("function", parent){}

CompilerFunctionState::~CompilerFunctionState()
{
}

CompilerState::StateStatus CompilerFunctionState::CheckIsNextState(char token)
{
    return CompilerState::StateContinue;
}
    
CompilerState::StateStatus CompilerFunctionState::CheckIsNextState(const std::string& word)
{
    return CompilerState::StateContinue;
}

CompilerState* CompilerFunctionState::GetNextState(const std::string& word)
{
    return 0;
}
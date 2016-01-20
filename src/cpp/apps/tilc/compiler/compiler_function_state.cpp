#include "compiler_function_state.h"

CompilerFunctionState::CompilerFunctionState(CompilerState* parent, const std::string& retVal, const std::string& funcName)
: CompilerState("function", parent)
{
    m_errorString.push_back("{");
    m_errorString.push_back("}");
    m_errorString.push_back("(");
    
    m_ignoreString.push_back("\r");
    m_ignoreString.push_back("\n");
}

CompilerFunctionState::~CompilerFunctionState(){}

CompilerState::StateStatus CompilerFunctionState::CheckIsNextState(char token)
{
    return CompilerState::StateContinue;
}
    
CompilerState::StateStatus CompilerFunctionState::CheckIsNextState(const std::string& word)
{
    m_checkWord = word;
    if(word == "//" || word == "/*")
    {
        return CompilerState::StateApply;
    }
    
    return CompilerState::StateContinue;
}

CompilerState* CompilerFunctionState::GetNextState(const std::string& word)
{
    return 0;
}
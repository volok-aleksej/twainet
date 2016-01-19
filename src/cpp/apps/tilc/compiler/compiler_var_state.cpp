#include "compiler_var_state.h"
#include "compiler_function_state.h"

CompilerVarState::CompilerVarState(CompilerState* parent, const std::string& varType)
: CompilerState("variable", parent), m_varType(varType), m_state(Name), m_lastSep(0)
{
    m_errorString.push_back("{");
    m_errorString.push_back("}");
    m_errorString.push_back(")");
    
    m_ignoreString.push_back("\r");
    m_ignoreString.push_back("\n");
}
CompilerVarState::~CompilerVarState()
{
}

CompilerState::StateStatus CompilerVarState::CheckIsNextState(char token)
{
    if(token == ',' || token == '(' || token == ';')
    {
        m_lastSep = token;
        return CompilerState::StateApply;
    }
    else if(token == ' ' || token == '\t')
    {
        if(m_state == Name)
        {
            return CompilerState::StateApply;
        }
        else
        {
            return CompilerState::StateIgnore;
        }
    }
    
    return CompilerState::StateContinue;
}

CompilerState::StateStatus CompilerVarState::CheckIsNextState(const std::string& word)
{
    if(m_state == Name)
    {
        m_varName = word;
        return CompilerState::StateContinue;
    }
    else
    {
        return CompilerState::StateError;
    }
}

CompilerState* CompilerVarState::GetNextState(const std::string& word)
{
    CompilerState* retState = 0;
    if (m_state == Name)
    {
        if(m_varName.empty() && !word.empty())
        {
            m_state = End;
            m_varName = word;
        }
        retState = this;
    }
    else if(!word.empty())
    {
        return 0;
    }
    
    if(m_state == End && m_lastSep == ',')
    {
        m_lastSep = 0;
        m_childState = new CompilerVarState(m_parentState, m_varType);
        return m_childState;
    }
    else if(m_state == End && m_lastSep == '(')
    {
        m_lastSep = 0;
        m_childState = new CompilerFunctionState(m_parentState, m_varType, m_varName);
        return m_childState;
    }
    else if(m_state == End && m_lastSep == ';')
    {
        return m_parentState;
    }
    
    return retState;
}
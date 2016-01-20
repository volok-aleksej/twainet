#include "compiler_var_state.h"
#include "compiler_function_state.h"
#include "compiler_comment_state.h"

CompilerVarState::CompilerVarState(CompilerState* parent, const std::string& varType)
: CompilerState("variable", parent), m_varType(varType), m_state(Name)
{
    m_useTokens.push_back(';');
    m_useTokens.push_back('(');
    m_useTokens.push_back(',');
    m_useTokens.push_back(' ');
    m_useTokens.push_back('\t');
    m_useTokens.push_back('\r');
    m_useTokens.push_back('\n');
}

CompilerVarState::~CompilerVarState(){}

CompilerState::StateStatus CompilerVarState::CheckIsUseWord(const std::string& word)
{
    if(word == "//" || word == "/*")
    {
        return CompilerState::StateApply;
    }
    
    return CompilerState::StateContinue;
}

CompilerState* CompilerVarState::GetNextState(const std::string& word, char token)
{
    if(word.empty() && token == 0)
        return this;
    else if(word == "//" || word == "/*")
    {
        m_childState = new CompilerCommentState(this, word);
        return m_childState;
    }
    else if(token == ' ' || token == '\t' ||
            token == '\r' || token == '\n')
    {
        if(word.empty())
            return this;
        else if(m_varName.empty())
        {
            m_state = End;
            m_varName = word;
            return this;
        }
    }
    else if(token == ',' || token == '(' || token == ';')
    {
        if(m_state == Name && word.empty() ||
           m_state == End && !word.empty())
            return 0;
        if(m_state == Name)
        {
            m_state = End;
            m_varName = word;
        }
        if(token == ',')
            m_childState = new CompilerVarState(m_parentState, m_varType);
        else if(token == '(')
            m_childState = new CompilerFunctionState(m_parentState, m_varType, m_varName);
        else 
            return m_parentState;
        
        return m_childState;
    }
    
    return 0;
}
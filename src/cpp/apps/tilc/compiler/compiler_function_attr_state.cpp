#include "compiler_function_attr_state.h"
#include "compiler_comment_state.h"

CompilerFunctionAttrState::CompilerFunctionAttrState(CompilerState* parent, const std::string& type)
: CompilerState("function_attr", parent), m_varType(type), m_state(Name)
{
    m_useTokens.push_back(' ');
    m_useTokens.push_back('\t');
    m_useTokens.push_back('\n');
    m_useTokens.push_back('\r');
    m_useTokens.push_back(',');
    m_useTokens.push_back(')');
}

CompilerFunctionAttrState::~CompilerFunctionAttrState(){}

CompilerState::StateStatus CompilerFunctionAttrState::CheckIsUseWord(const std::string& word)
{
    if(word == "//" || word == "/*")
    {
        return CompilerState::StateApply;
    }

    return CompilerState::StateContinue;
}

CompilerState* CompilerFunctionAttrState::GetNextState(const std::string& word, char token)
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
        if(m_varName.empty())
        {
            m_state = End;
            m_varName = word;
        }
        return this;
    }
    else if(token == ',' || token == ')')
    {
        if(m_state == Name && word.empty() ||
           m_state == End && !word.empty())
            return 0;
        else if(m_state == Name)
        {
            m_state = End;
            m_varName = word;
        }
        
        if(token == ',')
            return m_parentState;
        else
            return m_parentState->NextState("", ')');
    }
    
    return 0;
}
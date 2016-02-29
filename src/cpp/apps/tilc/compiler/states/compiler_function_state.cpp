#include "compiler_function_state.h"
#include "compiler_comment_state.h"
#include "compiler_function_attr_state.h"
#include "../til_compiler.h"
#include "types.h"

CompilerFunctionState::CompilerFunctionState(CompilerState* parent, ICompilerEvent* event, const std::string& retVal, const std::string& funcName)
: CompilerState("function", parent, event), m_retVal(retVal), m_funcName(funcName)
{
    m_useTokens.push_back(' ');
    m_useTokens.push_back('\t');
    m_useTokens.push_back('\n');
    m_useTokens.push_back('\r');
    m_useTokens.push_back(')');
}

CompilerFunctionState::~CompilerFunctionState(){}
    
CompilerState::StateStatus CompilerFunctionState::CheckIsUseWord(const std::string& word)
{
    if(word == "//" || word == "/*")
    {
        return CompilerState::StateApply;
    }
    
    return CompilerState::StateContinue;
}

CompilerState* CompilerFunctionState::GetNextState(const std::string& word, char token)
{
    if(word.empty() && token == 0)
        return this;
    else if(word == "//" || word == "/*")
    {
        m_childState = new CompilerCommentState(this, m_event, word);
        return m_childState;
    }
    else if(token == ' ' || token == '\t' ||
            token == '\r' || token == '\n')
    {
        if(word.empty())
            return this;
        else if(TypesManager::IsInType(word))
        {
            m_childState = new CompilerFunctionAttrState(this, m_event, word);
            return m_childState;
        }
    }
    else if(token == ')')
    {
        m_event->onFunctionEnd();
        return m_parentState;
    }
    
    return 0;
}
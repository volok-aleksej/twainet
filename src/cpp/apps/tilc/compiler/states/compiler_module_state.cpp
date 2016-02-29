#include "compiler_module_state.h"
#include "compiler_comment_state.h"
#include "compiler_var_state.h"
#include "../til_compiler.h"
#include "types.h"

CompilerModuleState::CompilerModuleState(CompilerState* parent, ICompilerEvent* event)
: CompilerState("module", parent, event), m_state(Name)
{
    m_useTokens.push_back('\r');
    m_useTokens.push_back('\n');
    m_useTokens.push_back('{');
    m_useTokens.push_back('}');
    m_useTokens.push_back(' ');
    m_useTokens.push_back(';');
    m_useTokens.push_back('\t');
}

CompilerModuleState::~CompilerModuleState(){}

CompilerState::StateStatus CompilerModuleState::CheckIsUseWord(const std::string& word)
{
    if(word == "//" || word == "/*")
    {
        return CompilerState::StateApply;
    }
    
    return CompilerState::StateContinue;
}

CompilerState* CompilerModuleState::GetNextState(const std::string& word, char token)
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
        else if(m_state == Name)
        {
            m_state = PreBody;
            m_moduleName = word;
            return this;
        }
        else if(m_state == PreBody)
            return 0;
        else if(m_state == Body &&
                TypesManager::IsInType(word))
        {
            m_childState = new CompilerVarState(this, m_event, word);
            return m_childState;
        }
    }
    else if(token == '{')
    {
        if (m_state == Name && word.empty() ||
            m_state == PreBody && !word.empty())
            return 0;
        else if(m_state == Name)
        {
            m_state = Body;
            m_moduleName = word;
        }
        else if(m_state == PreBody)
            m_state = Body;
        else
            return 0;
        
        m_event->onModuleBegin(m_moduleName);
        return this;
    }
    else if(token == ';')
    {
        if(m_state == Body && word.empty())
            return this;
        else if(m_state == Name && !word.empty())
        {
            m_state = Body;
            m_moduleName = word;
        }
        else if(m_state == PreBody && word.empty())
            m_state = Body;
        else
            return 0;
        
        m_event->onModuleBegin(m_moduleName);
        m_event->onModuleEnd();
        return m_parentState;
    }
    else if(token == '}' && m_state == Body)
    {
        m_event->onModuleEnd();
        return m_parentState;
    }
    
    return 0;
}
#include "tokenizer.h"
#include "compiler_state.h"
#include <string.h>
#include <iostream>

Tokenizer::Tokenizer(TILCompiler* compiler, CompilerState* currentState)
: m_compiler(compiler), m_currentState(currentState)
{
    m_reservedWords.push_back("//");
    m_reservedWords.push_back("/*");
    m_reservedWords.push_back("*/");
}

bool Tokenizer::ProcessToken(char token)
{
    typeToken type = GetType(token);
    if(type == typeSymbol)
    {
        m_lastString += m_lastReserved;
        m_lastReserved.clear();
        m_lastString.push_back(token);
    }
    else if(type == typeReserved)
    {
        CompilerState::StateStatus state = m_currentState->IsUseWord(m_lastReserved);
        if(state == CompilerState::StateApply)
        {
            m_currentState = m_currentState->NextState(m_lastString, 0);
            if(!m_currentState)
                return false;
            m_currentState = m_currentState->NextState(m_lastReserved, 0);
            m_lastReserved.clear();
            m_lastString.clear();
        }
        else if(state == CompilerState::StateError)
        {
            m_lastReserved.clear();
            m_lastString.clear();
            return false;
        }
        else if(state == CompilerState::StateIgnore)
        {
            m_lastReserved.clear();
        }
        else
        {
            m_lastString += m_lastReserved;
            m_lastReserved.clear();
        }
    }
    else if(type == typeSeporator)
    {
        m_lastString += m_lastReserved;
        m_lastReserved.clear();
        
        if(m_currentState->IsUseToken(token))
        {
            m_currentState = m_currentState->NextState(m_lastString, token);
            m_lastString.clear();
        }
        else
        {
            m_lastString.push_back(token);
        }
    }
    
    if(!m_currentState)
    {
        return false;
    }
    return true;
}

Tokenizer::typeToken Tokenizer::GetType(char ch)
{
    switch(ch)
    {
        case ' ':
        case '\r':
        case '\n':
        case '\t':
        case ',':
        case ';':
        case '{':
        case '}':
        case '(':
        case ')':
            return typeSeporator;
    }
    
    std::string data = m_lastReserved;
    do
    {
        data.push_back(ch);
        for(std::vector<std::string>::iterator it = m_reservedWords.begin();
            it != m_reservedWords.end(); it++)
        {
            if(data.size() < it->size() && memcmp(data.c_str(), it->c_str(), data.size()) == 0)
            {
                m_lastReserved.push_back(ch);
                return typeNeedMore;
            }
            else if(data.size() == it->size() && memcmp(data.c_str(), it->c_str(), data.size()) == 0)
            {
                m_lastReserved.push_back(ch);
                return typeReserved;
            }
        }
        
        if(!m_lastReserved.empty())
            data.clear();
	m_lastString += m_lastReserved;
        m_lastReserved.clear();
    }
    while(data.size() != 1 && ch != data[0]);

    return typeSymbol;
}
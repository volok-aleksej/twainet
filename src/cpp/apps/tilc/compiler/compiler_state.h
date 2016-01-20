#ifndef COMPILER_STATE_H
#define COMPILER_STATE_H

#include <string>
#include <vector>
#include <algorithm>

class CompilerState
{
public:
    enum StateStatus
    {
        StateApply,
        StateIgnore,
        StateContinue,
        StateError
    };
    
    CompilerState(const std::string& stateName, CompilerState* parentState)
    : m_stateName(stateName), m_parentState(parentState), m_childState(0){}
    virtual ~CompilerState()
    {
        if(m_childState)
        {
            delete m_childState;
            m_childState = 0;
        }
    }
    
    std::string GetStateName()
    {
        return m_stateName;
    }
    
    bool IsUseToken(char token)
    {
        std::vector<char>::iterator it = std::find(m_useTokens.begin(), m_useTokens.end(), token);
        return it != m_useTokens.end();
    }
    
    StateStatus IsUseWord(const std::string& word)
    {
        return CheckIsUseWord(word);
    }
    
    CompilerState* NextState(const std::string& word, char token)
    {
        CompilerState* state = GetNextState(word, token);
        if(state)
        {
            state->EnterState();
        }
        
        return state;
    }
    
protected:
    virtual StateStatus CheckIsUseWord(const std::string& word) = 0;
    virtual CompilerState* GetNextState(const std::string& word, char token) = 0;

    void EnterState()
    {
        if(m_childState)
        {
            delete m_childState;
            m_childState = 0;
        }
    }
    
protected:
    CompilerState* m_parentState;
    CompilerState* m_childState;
    std::vector<char> m_useTokens;
private:
    std::string m_stateName;
};

#endif/*COMPILER_STATE_H*/
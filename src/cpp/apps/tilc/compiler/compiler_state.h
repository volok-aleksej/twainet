#ifndef COMPILER_STATE_H
#define COMPILER_STATE_H

#include <string>
#include <vector>

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
    virtual StateStatus IsNextState(char token)
    {
        for(std::vector<std::string>::iterator it = m_errorString.begin();
            it != m_errorString.end(); it++)
        {
            if(*it == std::string(&token, 1))
                return StateError;
        }
        
        for(std::vector<std::string>::iterator it = m_ignoreString.begin();
            it != m_ignoreString.end(); it++)
        {
            if(*it == std::string(&token, 1))
                return StateIgnore;
        }
        
        return CheckIsNextState(token);
    }
    
    virtual StateStatus IsNextState(const std::string& word)
    {
        for(std::vector<std::string>::iterator it = m_errorString.begin();
            it != m_errorString.end(); it++)
        {
            if(*it == word)
                return StateError;
        }
        
        for(std::vector<std::string>::iterator it = m_ignoreString.begin();
            it != m_ignoreString.end(); it++)
        {
            if(*it == word)
                return StateIgnore;
        }
        
        return CheckIsNextState(word);
    }

    virtual CompilerState* NextState(const std::string& word)
    {
        CompilerState* state = GetNextState(word);
        if(state)
        {
            state->EnterState();
        }
        
        return state;
    }
    
protected:
    virtual StateStatus CheckIsNextState(char token) = 0;
    virtual StateStatus CheckIsNextState(const std::string& word) = 0;
    virtual CompilerState* GetNextState(const std::string& word) = 0;

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
    
    std::vector<std::string> m_errorString;
    std::vector<std::string> m_ignoreString;

private:
    std::string m_stateName;
};

#endif/*COMPILER_STATE_H*/
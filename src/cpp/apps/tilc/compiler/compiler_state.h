#ifndef COMPILER_STATE_H
#define COMPILER_STATE_H

#include <string>

class CompilerState
{
public:
    enum StateStatus
    {
        StateApply,
        StateIgnore,
        StateContinue
    };
    
    CompilerState(const std::string& stateName, CompilerState* parentState)
    : m_stateName(stateName), m_parentState(parentState), m_childState(0){}
    virtual ~CompilerState(){}
    
    std::string GetStateName()
    {
        return m_stateName;
    }
    virtual StateStatus IsNextState(char token)
    {
        return CheckIsNextState(token);
    }
    
    virtual StateStatus IsNextState(const std::string& word)
    {
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

private:
    std::string m_stateName;
};

#endif/*COMPILER_STATE_H*/
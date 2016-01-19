#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <string>
#include <vector>

class TILCompiler;
class CompilerState;

class Tokenizer
{
    enum typeToken
    {
        typeSymbol = 0,
        typeSeporator,
        typeReserved,
        typeNeedMore
    };
public:
    Tokenizer(TILCompiler* compiler, CompilerState* currentState);
    
    bool ProcessToken(char token);
protected:
    typeToken GetType(char ch);
private:
    std::string m_lastString;
    std::string m_lastReserved;
    std::vector<std::string> m_reservedWords;
    TILCompiler* m_compiler;
    CompilerState* m_currentState;
};

#endif/*TOKENIZER_H*/
#ifndef TIL_COMPILER_H
#define TIL_COMPILER_H

#include <string>
#include "tokenizer.h"
#include "compiler_root_state.h"

class CompilerState;

class TILCompiler
{
public:
    TILCompiler(const std::string& file);
    ~TILCompiler();
    
    bool Parse();

private:
    std::string m_file;
    Tokenizer m_tokenizer;
    CompilerRootState m_genericState;
};

#endif/*TIL_COMPILER_H*/
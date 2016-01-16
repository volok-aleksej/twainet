#include "til_compiler.h"
#include "common/file.h"
#include "compiler_state.h"

TILCompiler::TILCompiler(const std::string& file)
: m_file(file), m_tokenizer(this, &m_genericState){}

TILCompiler::~TILCompiler(){}

bool TILCompiler::Parse()
{
    File f(m_file);
    f.Open("rt");
    int sizeFile = f.GetFileSize(), currentPos = 0;
    while(currentPos < sizeFile)
    {
        unsigned int len = 1;
        char ch;
        if (!f.Read(&ch, &len) ||
            !m_tokenizer.ProcessToken(ch))
        {
            break;
        }

        currentPos++;
    }
    return false;
}
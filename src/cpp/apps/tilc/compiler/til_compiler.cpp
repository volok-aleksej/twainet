#include "til_compiler.h"
#include "common/file.h"

TILCompiler::TILCompiler(const std::string& file)
: m_file(file), m_genericState(this), m_tokenizer(this, &m_genericState){}

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
            return false;
        }

        currentPos++;
    }
    return true;
}

void TILCompiler::onPluginBegin(const std::string& name)
{
}

void TILCompiler::onPluginEnd()
{
}

void TILCompiler::onApplicationBegin(const std::string& name)
{
}

void TILCompiler::onApplicationEnd()
{
}

void TILCompiler::onModuleBegin(const std::string& name)
{
}

void TILCompiler::onModuleEnd()
{
}

void TILCompiler::onVariable(const std::string& type, const std::string& name)
{
}

void TILCompiler::onFunctionBegin(const std::string& type, const std::string& name)
{
}

void TILCompiler::onFunctionEnd()
{
}

void TILCompiler::onComment(const std::string& data)
{
}

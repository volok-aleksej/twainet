#ifndef TIL_COMPILER_H
#define TIL_COMPILER_H

#include <string>
#include "tokenizer.h"
#include "states/compiler_root_state.h"

class CompilerState;

class ICompilerEvent
{
public:
    virtual void onPluginBegin(const std::string& name) = 0;
    virtual void onPluginEnd() = 0;
    virtual void onApplicationBegin(const std::string& name) = 0;
    virtual void onApplicationEnd() = 0;
    virtual void onModuleBegin(const std::string& name) = 0;
    virtual void onModuleEnd() = 0;
    virtual void onVariable(const std::string& type, const std::string& name) = 0;
    virtual void onFunctionBegin(const std::string& type, const std::string& name) = 0;
    virtual void onFunctionEnd() = 0;
    virtual void onComment(const std::string& data) = 0;
};

class TILCompiler : public ICompilerEvent
{
public:
    TILCompiler(const std::string& file);
    ~TILCompiler();
    
    bool Parse();
protected:
    virtual void onPluginBegin(const std::string& name);
    virtual void onPluginEnd();
    virtual void onApplicationBegin(const std::string& name);
    virtual void onApplicationEnd();
    virtual void onModuleBegin(const std::string& name);
    virtual void onModuleEnd();
    virtual void onVariable(const std::string& type, const std::string& name);
    virtual void onFunctionBegin(const std::string& type, const std::string& name);
    virtual void onFunctionEnd();
    virtual void onComment(const std::string& data);
private:
    std::string m_file;
    Tokenizer m_tokenizer;
    CompilerRootState m_genericState;
};

#endif/*TIL_COMPILER_H*/
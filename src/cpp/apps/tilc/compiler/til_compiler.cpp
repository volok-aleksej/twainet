#include "til_compiler.h"
#include "common/file.h"

TILCompiler::TILCompiler(const std::string& file)
: m_file(file), m_genericState(this)
, m_tokenizer(this, &m_genericState)
, m_rootObject(TIObject::Root, "")
, m_currentObject(&m_rootObject){}

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

TIObject* TILCompiler::getTIObject()
{
    return &m_rootObject;
}

void TILCompiler::onPluginBegin(const std::string& name)
{
    TIObject* object = new ContainerObject(ContainerObject::Plugin, name);
    m_currentObject->AddChild(object);
    m_currentObject = object;
}

void TILCompiler::onPluginEnd()
{
    m_currentObject = m_currentObject->GetParent();
}

void TILCompiler::onApplicationBegin(const std::string& name)
{
    TIObject* object = new ContainerObject(ContainerObject::Application, name);
    m_currentObject->AddChild(object);
    m_currentObject = object;
}

void TILCompiler::onApplicationEnd()
{
    m_currentObject = m_currentObject->GetParent();
}

void TILCompiler::onModuleBegin(const std::string& name)
{
    TIObject* object = new ContainerObject(ContainerObject::Module, name);
    m_currentObject->AddChild(object);
    m_currentObject = object;
}

void TILCompiler::onModuleEnd()
{
    m_currentObject = m_currentObject->GetParent();
}

void TILCompiler::onVariable(const std::string& type, const std::string& name)
{
    TIObject* object = new RetObject(RetObject::Variable, name, type);
    m_currentObject->AddChild(object);
}

void TILCompiler::onFunctionBegin(const std::string& type, const std::string& name)
{
    TIObject* object = new RetObject(RetObject::Function, name, type);
    m_currentObject->AddChild(object);
    m_currentObject = object;
}

void TILCompiler::onFunctionEnd()
{
    m_currentObject = m_currentObject->GetParent();
}

void TILCompiler::onComment(const std::string& data)
{
}

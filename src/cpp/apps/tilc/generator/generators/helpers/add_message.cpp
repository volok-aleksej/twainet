#include "add_message.h"
#include "types.h"

AddMessageStub::AddMessageStub()
{
}

AddMessageStub::~AddMessageStub()
{
}
   
std::string AddMessageStub::GenerateH(TIObject* object)
{
    return "";
}

std::string AddMessageStub::GenerateCPP(TIObject* object)
{
    std::string result;
    RetObject* retObject = dynamic_cast<RetObject*>(object);
    if(!object || !retObject)
    {
        return result;
    }

    result.append("    AddMessage(new ");
    result.append(object->GetName());
    result.append("Message(this));\n");
    return result;
}

std::string AddMessageStub::GenerateProto(TIObject* object)
{
    return "";
}


AddMessageProxy::AddMessageProxy()
{
}

AddMessageProxy::~AddMessageProxy()
{
}
   
std::string AddMessageProxy::GenerateH(TIObject* object)
{
    return "";
}

std::string AddMessageProxy::GenerateCPP(TIObject* object)
{
    std::string result;
    RetObject* retObject = dynamic_cast<RetObject*>(object);
    if(!object || !retObject)
    {
        return result;
    }

    result.append("    m_module->AddMessage(new ");
    result.append(object->GetName());
    result.append("ResultMessage(this));\n");
    return result;
}

std::string AddMessageProxy::GenerateProto(TIObject* object)
{
    return "";
}
#include "content_proxy.h"
#include "types.h"

ContentProxy::ContentProxy()
{
}

ContentProxy::~ContentProxy()
{
}
   
std::string ContentProxy::GenerateH(TIObject* object)
{
    std::string result;
    RetObject* retObject = dynamic_cast<RetObject*>(object);
    if(!object || !retObject)
    {
        return result;
    }
    
    result.append("    typedef DeamonMessage<");
    result.append(retObject->GetName());
    result.append("_result,");
    result.append(object->GetParent()->GetName() + MODULE_CLIENT_POSTFIX);
    result.append("> ");
    result.append(retObject->GetName());
    result.append("ResultMessage;\n    ");
    result.append("void onMessage(const ");
    result.append(object->GetName());
    result.append("_result& msg, const Twainet::ModuleName& path);\n");
    return result;
}

std::string ContentProxy::GenerateCPP(TIObject* object)
{
    std::string result;
    RetObject* retObject = dynamic_cast<RetObject*>(object);
    if(!object || !retObject)
    {
        return result;
    }

    result.append("void ");
    result.append(retObject->GetParent()->GetName() + MODULE_CLIENT_POSTFIX);
    result.append("::onMessage(const ");
    result.append(retObject->GetName());
    result.append("_result& msg, const Twainet::ModuleName& path)\n{\n    ");
    result.append("Semaphore* sm = m_requests[msg.id()];\n    ");
    result.append("if(sm)\n    ");
    result.append("{\n      ");
    result.append("sm->Release();\n    ");
    result.append("}\n");
    result.append("}\n");
    return result;
}

std::string ContentProxy::GenerateProto(TIObject* object)
{
    return "";
}
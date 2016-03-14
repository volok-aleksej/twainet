#include "function_proxy.h"
#include "types.h"

FunctionProxy::FunctionProxy()
{
}

FunctionProxy::~FunctionProxy()
{
}
   
std::string FunctionProxy::GenerateH(TIObject* object)
{
    std::string result;
    RetObject* retObject = dynamic_cast<RetObject*>(object);
    if(!object || !retObject)
    {
        return result;
    }
    
    result.append("    ");
    result.append(TypesManager::GetCType(retObject->GetRetValue()));
    result.append(" ");
    result.append(retObject->GetName());
    result.append("(");
    std::vector<TIObject*> childs = object->GetChilds();
    for(std::vector<TIObject*>::iterator it = childs.begin();
        it != childs.end(); it++)
    {
        RetObject* varObject = dynamic_cast<RetObject*>(*it);
        if(varObject)
        {
            result.append(TypesManager::GetCType(varObject->GetRetValue()));
            result.append(" ");
            result.append(varObject->GetName());
            result.append(",");
        }
    }
    result.append("const Twainet::ModuleName& path);\n");
    return result;
}

std::string FunctionProxy::GenerateCPP(TIObject* object)
{
    std::string result;
    RetObject* retObject = dynamic_cast<RetObject*>(object);
    if(!object || !retObject)
    {
        return result;
    }
    
    result.append(TypesManager::GetCType(retObject->GetRetValue()));
    result.append(" ");
    result.append(retObject->GetParent()->GetName() + MODULE_CLIENT_POSTFIX);
    result.append("::");
    result.append(retObject->GetName());
    result.append("(");
    std::string fillingMsg("msg.set_id(rand());\n    ");
    std::vector<TIObject*> childs = object->GetChilds();
    for(std::vector<TIObject*>::iterator it = childs.begin();
        it != childs.end(); it++)
    {
        RetObject* varObject = dynamic_cast<RetObject*>(*it);
        if(varObject)
        {
            result.append(TypesManager::GetCType(varObject->GetRetValue()));
            result.append(" ");
            result.append(varObject->GetName());
            result.append(",");
            fillingMsg.append("msg.set_arg_");
            fillingMsg.append(varObject->GetName());
            fillingMsg.append("(");
            fillingMsg.append(varObject->GetName());
            fillingMsg.append(");\n    ");
        }
    }
    result.append("const Twainet::ModuleName& path)\n{\n    ");
    result.append(retObject->GetParent()->GetName() + MODULE_SERVER_POSTFIX);
    result.append("::");
    result.append(retObject->GetName());
    result.append("Message msg(0);\n    ");
    result.append(fillingMsg);
    result.append("m_module->toMessage(msg, path);\n}\n");
    return result;
}

std::string FunctionProxy::GenerateProto(TIObject* object)
{
    return "";
}
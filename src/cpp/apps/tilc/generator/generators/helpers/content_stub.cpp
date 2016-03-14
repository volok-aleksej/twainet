#include "content_stub.h"
#include "types.h"

ContentStub::ContentStub()
{
}

ContentStub::~ContentStub()
{
}
   
std::string ContentStub::GenerateH(TIObject* object)
{
    std::string result;
    RetObject* retObject = dynamic_cast<RetObject*>(object);
    if(!object || !retObject)
    {
        return result;
    }
    
    result.append("    typedef DeamonMessage<");
    result.append(retObject->GetName());
    result.append("_,");
    result.append(object->GetParent()->GetName() + MODULE_SERVER_POSTFIX);
    result.append("> ");
    result.append(retObject->GetName());
    result.append("Message;\n    ");
    result.append("void onMessage(const ");
    result.append(object->GetName());
    result.append("_& msg, const Twainet::ModuleName& path);\n    ");
    result.append("void fireReturn(const ");
    result.append(object->GetName());
    result.append("_result& msg, const Twainet::ModuleName& path);\n");
    return result;
}

std::string ContentStub::GenerateCPP(TIObject* object)
{
    std::string result;
    RetObject* retObject = dynamic_cast<RetObject*>(object);
    if(!object || !retObject)
    {
        return result;
    }

    std::string onMessageContent;
    std::string protoRetType = TypesManager::GetProtoType(retObject->GetRetValue());
    if(!protoRetType.empty())
    {
        onMessageContent.append(TypesManager::GetCType(retObject->GetRetValue()));
        onMessageContent.append(" ret = ");
    }
    onMessageContent.append(retObject->GetName());
    onMessageContent.append("(");
    std::vector<TIObject*> childs = object->GetChilds();
    for(std::vector<TIObject*>::iterator it = childs.begin();
        it != childs.end(); it++)
    {
        RetObject* varObject = dynamic_cast<RetObject*>(*it);
        if(varObject)
        {
            if(it != childs.begin())
            {
                onMessageContent.append(",");
            }
            onMessageContent.append("msg.arg_");
            onMessageContent.append(varObject->GetName());
            onMessageContent.append("()");
        }
    }
    onMessageContent.append(");\n    ");
    onMessageContent.append(retObject->GetName());
    onMessageContent.append("_result retMsg;\n    ");
    onMessageContent.append("retMsg.set_id(msg.id());\n    ");
    if(!protoRetType.empty())
    {
        onMessageContent.append("retMsg.set_result(ret);\n    ");
    }
    onMessageContent.append("fireReturn(retMsg, path);\n");
    
    result.append("void ");
    result.append(retObject->GetParent()->GetName() + MODULE_SERVER_POSTFIX);
    result.append("::onMessage(const ");
    result.append(retObject->GetName());
    result.append("_& msg, const Twainet::ModuleName& path)\n{\n    ");
    result.append(onMessageContent);
    result.append("}\n");
    result.append("void ");
    result.append(retObject->GetParent()->GetName() + MODULE_SERVER_POSTFIX);
    result.append("::fireReturn(const ");
    result.append(retObject->GetName());
    result.append("_result& msg, const Twainet::ModuleName& path)\n{\n    ");
    result.append(retObject->GetParent()->GetName() + MODULE_CLIENT_POSTFIX);
    result.append("::");
    result.append(retObject->GetName());
    result.append("ResultMessage retMsg(0, msg);\n    ");
    result.append("toMessage(retMsg, path);\n");
    result.append("}\n");
    return result;
}

std::string ContentStub::GenerateProto(TIObject* object)
{
    return "";
}
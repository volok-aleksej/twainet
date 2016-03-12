#include "function_generator.h"
#include "types.h"
#include "../generator_manager.h"
#include <stdio.h>

FunctionGenerator::FunctionGenerator()
{
}

FunctionGenerator::~FunctionGenerator()
{
}
    
std::string FunctionGenerator::GenerateH(TIObject* object, const std::string& parameter)
{
    std::string result;
    RetObject* retObject = dynamic_cast<RetObject*>(object);
    if(retObject && parameter == CONTENT_DECLARE_STUB_TMPL)
    {
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
    }
    if(retObject && parameter == CONTENT_DECLARE_PROXY_TMPL)
    {
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
    }
    else if(retObject && parameter == FUNCTIONS_PROXY_TMPL)
    {
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
    }
    else if(retObject && parameter == FUNCTIONS_STUB_TMPL)
    {
        result.append("    virtual ");
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
                if(it != childs.begin())
                {
                    result.append(",");
                }
                result.append(TypesManager::GetCType(varObject->GetRetValue()));
                result.append(" ");
                result.append(varObject->GetName());
            }
        }
        result.append(");\n");
    }
    return result;
}

std::string FunctionGenerator::GenerateCPP(TIObject* object, const std::string& parameter)
{
    std::string result;
    RetObject* retObject = dynamic_cast<RetObject*>(object);
    if(retObject && parameter == CONTENT_DECLARE_STUB_TMPL)
    {
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
                onMessageContent.append("msg.");
                onMessageContent.append(varObject->GetName());
                onMessageContent.append("()");
            }
        }
        onMessageContent.append(");\n    ");
        onMessageContent.append(retObject->GetName());
        onMessageContent.append("_result retMsg;\n    ");
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
    }
    else if(retObject && parameter == CONTENT_DECLARE_PROXY_TMPL)
    {
        std::string onMessageContent("\n");
        result.append("void ");
        result.append(retObject->GetParent()->GetName() + MODULE_CLIENT_POSTFIX);
        result.append("::onMessage(const ");
        result.append(retObject->GetName());
        result.append("_result& msg, const Twainet::ModuleName& path)\n{\n    ");
        result.append(onMessageContent);
        result.append("}\n");
    }
    else if(retObject && parameter == ADD_MESSAGES_TMPL)
    {
        result.append("    AddMessage(new ");
        result.append(object->GetName());
        result.append("Message(this));\n");
    }
    else if(retObject && parameter == FUNCTIONS_STUB_TMPL)
    {
        result.append(TypesManager::GetCType(retObject->GetRetValue()));
        result.append(" ");
        result.append(retObject->GetParent()->GetName() + MODULE_SERVER_POSTFIX);
        result.append("::");
        result.append(retObject->GetName());
        result.append("(");
        std::vector<TIObject*> childs = object->GetChilds();
        for(std::vector<TIObject*>::iterator it = childs.begin();
            it != childs.end(); it++)
        {
            RetObject* varObject = dynamic_cast<RetObject*>(*it);
            if(varObject)
            {
                if(it != childs.begin())
                {
                    result.append(",");
                }                
                result.append(TypesManager::GetCType(varObject->GetRetValue()));
                result.append(" ");
                result.append(varObject->GetName());
            }
        }
        result.append(")\n{\n    ");
        result.append(FUNCTIONS_APP_TMPL);
        result.append("\n}\n");
    }
    else if(retObject && parameter == FUNCTIONS_PROXY_TMPL)
    {
        result.append(TypesManager::GetCType(retObject->GetRetValue()));
        result.append(" ");
        result.append(retObject->GetParent()->GetName() + MODULE_CLIENT_POSTFIX);
        result.append("::");
        result.append(retObject->GetName());
        result.append("(");
        std::string fillingMsg;
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
                fillingMsg.append("msg.set_");
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
    }
    else if(retObject && parameter == FUNCTIONS_APP_TMPL)
    {
        if(!TypesManager::GetDefaultReturn(retObject->GetRetValue()).empty())
        {
            result.append("return ");
        }
        
        result.append("m_app->");
        result.append(retObject->GetName());
        result.append("(");
        std::vector<TIObject*> childs = object->GetChilds();
        for(std::vector<TIObject*>::iterator it = childs.begin();
            it != childs.end(); it++)
        {
            RetObject* varObject = dynamic_cast<RetObject*>(*it);
            if(varObject)
            {
                if(it != childs.begin())
                {
                    result.append(",");
                }
                result.append(varObject->GetName());
            }
        }
        result.append(");");
    }
    return result;
}

std::string FunctionGenerator::GenerateProto(TIObject* object, const std::string& parameter)
{
    std::string result("message ");
    result.append(object->GetName());
    result.append("_ {\n");    
    std::vector<TIObject*> childs = object->GetChilds();
    for(std::vector<TIObject*>::iterator it = childs.begin();
        it != childs.end(); it++)
    {
        RetObject* varObject = dynamic_cast<RetObject*>(*it);
        if(varObject)
        {
            std::string rtype = TypesManager::GetProtoType(varObject->GetRetValue());
            if(!rtype.empty())
            {
                char num[10] = {0};
                sprintf(num, "%d", int(it - childs.begin()) + 1);
                result.append("    required ");
                result.append(rtype);
                result.append(" ");
                result.append(varObject->GetName());
                result.append(" = ");
                result.append(num);
                result.append(";\n");
            }
        }
    }
    result.append("}\n");
    
    RetObject* retObject = dynamic_cast<RetObject*>(object);
    std::string rtype = TypesManager::GetProtoType(retObject->GetRetValue());
    result.append("message ");
    result.append(object->GetName());
    result.append("_result {\n");
    if(!rtype.empty())
    {
        result.append("    required ");
        result.append(rtype);
        result.append(" result = 1;\n");
    }
    result.append("}\n");
    return result;
}

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
    if(retObject && parameter == CONTENT_DECLARE_TMPL)
    {
        result.append("    typedef DeamonMessage<");
        result.append(retObject->GetName());
        result.append("_,");
        result.append(object->GetParent()->GetName() + MODULE_POSTFIX);
        result.append("> ");
        result.append(retObject->GetName());
        result.append("Message;\n    ");
        result.append("void onMessage(const ");
        result.append(object->GetName());
        result.append("_& msg, const Twainet::ModuleName& path);\n");
    }
    else if(retObject && parameter == FUNCTIONS_TMPL)
    {
        result.append("     virtual ");
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
    if(retObject && parameter == CONTENT_DECLARE_TMPL)
    {
        
        result.append("void ");
        result.append(retObject->GetParent()->GetName() + MODULE_POSTFIX);
        result.append("::onMessage(const ");
        result.append(retObject->GetName());
        result.append("_& msg, const Twainet::ModuleName& path)\n{\n    ");
        result.append(retObject->GetName());
        result.append("(");
        std::vector<TIObject*> childs = object->GetChilds();
        for(std::vector<TIObject*>::iterator it = childs.begin();
            it != childs.end(); it++)
        {
            if(it != childs.begin())
            {
                result.append(",");
            }
            RetObject* varObject = dynamic_cast<RetObject*>(*it);
            if(varObject)
            {
                result.append("msg.");
                result.append(varObject->GetName());
                result.append("()");
            }
        }
        result.append(");\n}\n");
    }
    else if(retObject && parameter == FUNCTIONS_TMPL)
    {
        result.append(TypesManager::GetCType(retObject->GetRetValue()));
        result.append(" ");
        result.append(retObject->GetParent()->GetName() + MODULE_POSTFIX);
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
    if(!rtype.empty())
    {
        result.append("message ");
        result.append(object->GetName());
        result.append("_return {\n    ");
        result.append("required ");
        result.append(rtype);
        result.append(" return = 1;\n}\n");
    }
            
    return result;
}

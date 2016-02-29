#include "function_generator.h"
#include "types.h"
#include "../generator_manager.h"

FunctionGenerator::FunctionGenerator()
{
}

FunctionGenerator::~FunctionGenerator()
{
}
    
std::string FunctionGenerator::GenerateH(TIObject* object)
{
    std::string result;
    RetObject* retObject = dynamic_cast<RetObject*>(object);
    if(retObject)
    {
        // generate virtual function
        result.append("public:\n    ");
        result.append("virtual ");
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
        result.append(") = 0;\n");
        
        // generate message function
        result.append("protected:\n    ");
        result.append("typedef DeamonMessage<");
        result.append(retObject->GetName());
        result.append(",");
        result.append(object->GetParent()->GetName() + "StubImpl");
        result.append("> ");
        result.append(retObject->GetName());
        result.append("Message;\n    ");
        result.append("onMessage(const ");
        result.append(object->GetName());
        result.append("& msg, const Twainet::ModuleName& path);\n");
    }
    return result;
}

std::string FunctionGenerator::GenerateCPP(TIObject* object)
{
    std::string result;
    return result;
}

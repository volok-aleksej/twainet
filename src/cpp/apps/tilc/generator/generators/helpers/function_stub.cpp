#include "function_stub.h"
#include "types.h"

FunctionStub::FunctionStub()
{
}

FunctionStub::~FunctionStub()
{
}
   
std::string FunctionStub::GenerateH(TIObject* object)
{
    std::string result;
    RetObject* retObject = dynamic_cast<RetObject*>(object);
    if(!object || !retObject)
    {
        return result;
    }
    
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
    return result;
}

std::string FunctionStub::GenerateCPP(TIObject* object)
{
    std::string result;
    RetObject* retObject = dynamic_cast<RetObject*>(object);
    if(!object || !retObject)
    {
        return result;
    }
    
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
    return result;
}

std::string FunctionStub::GenerateProto(TIObject* object)
{
    return "";
}
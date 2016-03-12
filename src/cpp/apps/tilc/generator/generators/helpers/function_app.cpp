#include "function_app.h"
#include "types.h"

FunctionApp::FunctionApp()
{
}

FunctionApp::~FunctionApp()
{
}
   
std::string FunctionApp::GenerateH(TIObject* object)
{
    return "";
}

std::string FunctionApp::GenerateCPP(TIObject* object)
{
    std::string result;
    RetObject* retObject = dynamic_cast<RetObject*>(object);
    if(!object || !retObject)
    {
        return result;
    }

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
    return result;
}

std::string FunctionApp::GenerateProto(TIObject* object)
{
    return "";
}
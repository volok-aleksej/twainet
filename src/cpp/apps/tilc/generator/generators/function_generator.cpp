#include "function_generator.h"
#include "types.h"
#include "../generator_manager.h"
#include "helpers/function_proxy.h"
#include "helpers/content_proxy.h"
#include "helpers/function_stub.h"
#include "helpers/content_stub.h"
#include "helpers/function_app.h"
#include "helpers/add_message.h"
#include <stdio.h>

FunctionGenerator::FunctionGenerator()
{
    m_helpers.insert(std::make_pair(FUNCTIONS_PROXY_TMPL, new FunctionProxy));
    m_helpers.insert(std::make_pair(FUNCTIONS_STUB_TMPL, new FunctionStub));
    m_helpers.insert(std::make_pair(CONTENT_DECLARE_PROXY_TMPL, new ContentProxy));
    m_helpers.insert(std::make_pair(CONTENT_DECLARE_STUB_TMPL, new ContentStub));
    m_helpers.insert(std::make_pair(FUNCTIONS_APP_TMPL, new FunctionApp));
    m_helpers.insert(std::make_pair(ADD_MESSAGES_STUB_TMPL, new AddMessageStub));
    m_helpers.insert(std::make_pair(ADD_MESSAGES_PROXY_TMPL, new AddMessageProxy));
}

FunctionGenerator::~FunctionGenerator()
{
    for(std::map<std::string, Helper*>::iterator it = m_helpers.begin();
        it != m_helpers.end(); it++)
    {
        delete it->second;
    }
}
    
std::string FunctionGenerator::GenerateH(TIObject* object, const std::string& parameter)
{
    std::string result;
    std::map<std::string, Helper*>::iterator it = m_helpers.find(parameter);
    if(it == m_helpers.end())
    {
        return result;
    }
    
    result = m_helpers[parameter]->GenerateH(object);
    return result;
}

std::string FunctionGenerator::GenerateCPP(TIObject* object, const std::string& parameter)
{
    std::string result;
    std::map<std::string, Helper*>::iterator it = m_helpers.find(parameter);
    if(it == m_helpers.end())
    {
        return result;
    }
    
    result = m_helpers[parameter]->GenerateCPP(object);
    return result;
}

std::string FunctionGenerator::GenerateProto(TIObject* object, const std::string& parameter)
{
    std::string result("message ");
    result.append(object->GetName());
    result.append("_ {\n");
    result.append("    required int32 id = 1;\n");
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
                sprintf(num, "%d", int(it - childs.begin()) + 2);
                result.append("    required ");
                result.append(rtype);
                result.append(" arg_");
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
    result.append("    required int32 id = 1;\n");
    if(!rtype.empty())
    {
        result.append("    required ");
        result.append(rtype);
        result.append(" result = 2;\n");
    }
    result.append("}\n");
    return result;
}

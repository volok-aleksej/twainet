#include "til_generator.h"
#include "../generator_manager.h"
#include "resource.h"

#include <string.h>
#include <algorithm>

TILGenerator::TILGenerator(const std::string& path)
: m_folderPath(path)
{
}

TILGenerator::~TILGenerator()
{
}

std::string TILGenerator::GenerateH(TIObject* object, const std::string& parameter)
{
    std::vector<TIObject*> childs = object->GetChilds();
    for(std::vector<TIObject*>::iterator it = childs.begin();
        it != childs.end(); it++)
    {
        Generator* generator = GeneratorManager::GetInstance().GetGenerator((*it)->GetType());
        if(generator)
        {
            generator->GenerateH(*it);
        }
    }
    
    return "";
}

std::string TILGenerator::GenerateCPP(TIObject* object, const std::string& parameter)
{
    std::vector<TIObject*> childs = object->GetChilds();
    for(std::vector<TIObject*>::iterator it = childs.begin();
        it != childs.end(); it++)
    {
        Generator* generator = GeneratorManager::GetInstance().GetGenerator((*it)->GetType());
        if(generator)
        {
            generator->GenerateCPP(*it);
        }
    }
    return "";
}

std::string TILGenerator::GenerateProto(TIObject* object, const std::string& parameter)
{
    std::vector<TIObject*> childs = object->GetChilds();
    for(std::vector<TIObject*>::iterator it = childs.begin();
        it != childs.end(); it++)
    {
        Generator* generator = GeneratorManager::GetInstance().GetGenerator((*it)->GetType());
        if(generator)
        {
            generator->GenerateProto(*it);
        }
    }
    
    LOAD_RESOURCE(apps_tilc_resources_deamon_proto, deamon_proto_str);
    std::string deamon_proto_data(deamon_proto_str.data(), deamon_proto_str.size());
    saveInFile(m_folderPath + "/deamon.proto", deamon_proto_data);
    
    return "";
}

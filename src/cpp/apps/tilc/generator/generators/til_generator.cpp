#include "til_generator.h"
#include "../generator_manager.h"
#include "resource.h"

#include <string.h>
#include <algorithm>

TILGenerator::TILGenerator()
{
}

TILGenerator::~TILGenerator()
{
}

std::string TILGenerator::GenerateH(TIObject* object)
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

std::string TILGenerator::GenerateCPP(TIObject* object)
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

std::string TILGenerator::GenerateProto(TIObject* object)
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
    return "";
}

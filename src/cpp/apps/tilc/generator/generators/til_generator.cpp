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
/*
void TILGenerator::generateModule(TIObject* object)
{
    //generate module header file
    LOAD_RESOURCE(apps_tilc_resources_module_h_tmpl, module_h_str);
    std::string module_h_data(module_h_str.data(), module_h_str.size());
    std::string defines("#define ");
    defines.append(getDefinedModuleName(object->GetName()));
    defines.append(" \"");
    defines.append(object->GetName());
    defines.append("\"");
    std::map<std::string, std::string> replacement_app_h;
    replacement_app_h.insert(std::make_pair(HEADER_TMPL, getIfnDefHeader(object->GetName())));
    replacement_app_h.insert(std::make_pair(INCLUDES_TMPL, ""));
    replacement_app_h.insert(std::make_pair(DEFINES_TMPL, defines));
    replacement_app_h.insert(std::make_pair(CLASS_NAME_TMPL, object->GetName()));
    loadAndReplace(replacement_app_h, module_h_data);
    saveInFile(m_folderPath + "/" + object->GetName() + ".h", module_h_data);
    
}*/

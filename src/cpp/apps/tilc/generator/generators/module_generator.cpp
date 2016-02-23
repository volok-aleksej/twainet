#include "module_generator.h"
#include "../generator_manager.h"
#include "resource.h"

ModuleGenerator::ModuleGenerator(const std::string& path)
: m_folderPath(path)
{
}

ModuleGenerator::~ModuleGenerator()
{
}
    
std::string ModuleGenerator::GenerateH(TIObject* object)
{
    std::string content;
    std::vector<TIObject*> childs = object->GetChilds();
    for(std::vector<TIObject*>::iterator it = childs.begin();
        it != childs.end(); it++)
    {
        Generator* generator = GeneratorManager::GetInstance().GetGenerator((*it)->GetType());
        if(generator)
        {
            content.append(generator->GenerateH(*it));
        }
    }
        
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
    replacement_app_h.insert(std::make_pair(CONTENT_DECLARE_TMPL, content));
    loadAndReplace(replacement_app_h, module_h_data);
    saveInFile(m_folderPath + "/" + object->GetName() + ".h", module_h_data);
    return module_h_data;
}

std::string ModuleGenerator::GenerateCPP(TIObject* object)
{
}
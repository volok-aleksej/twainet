#include "app_generator.h"
#include "../generator_manager.h"
#include "resource.h"

#include <stdio.h>

AppGenerator::AppGenerator(const std::string& path)
: m_folderPath(path)
{
}

AppGenerator::~AppGenerator()
{
}
    
std::string AppGenerator::GenerateH(TIObject* object, const std::string& parameter)
{
    std::string functions;
    std::vector<TIObject*> childs = object->GetChilds();
    for(std::vector<TIObject*>::iterator it = childs.begin();
        it != childs.end(); it++)
    {
        if((*it)->GetType() == TIObject::Module)
        {
            Generator* generator = GeneratorManager::GetInstance().GetGenerator(TIObject::Module);
            if(generator) {
                generator->GenerateH(*it);
            }
        }
        else if((*it)->GetType() == TIObject::Variable ||
                (*it)->GetType() == TIObject::Function)
        {            
            Generator* generator = GeneratorManager::GetInstance().GetGenerator((*it)->GetType());
            if(generator) {
                functions.append(generator->GenerateH(*it, FUNCTIONS_TMPL));
            }
        }
    }

    Generator* generator = GeneratorManager::GetInstance().GetGenerator(TIObject::Module);
    if(generator) {
        generator->GenerateH(object, APP_TMPL);
    }

    LOAD_RESOURCE(apps_tilc_resources_application_h_tmpl, app_h_str);
    std::string app_h_data(app_h_str.data(), app_h_str.size());
    std::map<std::string, std::string> replacement_app_h;
    replacement_app_h.insert(std::make_pair(HEADER_TMPL, getIfnDefHeader(object->GetName())));
    replacement_app_h.insert(std::make_pair(CLASS_NAME_TMPL, object->GetName()));
    replacement_app_h.insert(std::make_pair(FUNCTIONS_TMPL, functions));
    loadAndReplace(replacement_app_h, app_h_data);    
    saveInFile(m_folderPath + "/" + object->GetName() + ".h", app_h_data);
    return app_h_data;
}

std::string AppGenerator::GenerateCPP(TIObject* object, const std::string& parameter)
{
    std::vector<std::string> modulesNames;
    std::vector<TIObject*> childs = object->GetChilds();
    for(std::vector<TIObject*>::iterator it = childs.begin();
        it != childs.end(); it++)
    {
        if((*it)->GetType() == TIObject::Module)
        {
            Generator* generator = GeneratorManager::GetInstance().GetGenerator(TIObject::Module);
            if(generator) {
                generator->GenerateCPP(*it);
            }
        }
    }
    
    Generator* generator = GeneratorManager::GetInstance().GetGenerator(TIObject::Module);
    if(generator) {
        generator->GenerateProto(object, APP_TMPL);
    }

    LOAD_RESOURCE(apps_tilc_resources_application_cpp_tmpl, app_cpp_str);
    std::string app_cpp_data(app_cpp_str.data(), app_cpp_str.size());
    std::string modules_headers, check_modules, create_modules;
    check_modules.append("\tif(strcmp(Twainet::GetModuleName(module).m_name, \"");
    check_modules.append(object->GetName());
    check_modules.append("\") == 0)\n\t{\n\t\t");
    check_modules.append(object->GetName());
    check_modules.append("::GetInstance().Stop();\n\t}");
    modules_headers.append("#include \"");
    modules_headers.append(object->GetName());
    modules_headers.append(MODULE_POSTFIX);
    modules_headers.append(".h\"\n");
    create_modules.append("\tAddModule(new ");
    create_modules.append(object->GetName());
    create_modules.append(MODULE_POSTFIX);
    create_modules.append("(this));");
    std::string header = object->GetName();
    header.append(".h");
    std::map<std::string, std::string> replacement_app_cpp;
    replacement_app_cpp.insert(std::make_pair(CLASS_NAME_TMPL, object->GetName()));
    replacement_app_cpp.insert(std::make_pair(MODULE_HEADERS_TMPL, modules_headers));
    replacement_app_cpp.insert(std::make_pair(HEADER_TMPL, header));
    replacement_app_cpp.insert(std::make_pair(CHECK_SERVER_TMPL, ""));
    replacement_app_cpp.insert(std::make_pair(CHECK_MODULES_TMPL, check_modules));
    replacement_app_cpp.insert(std::make_pair(CREATE_MODULES_TMPL, create_modules));
    replacement_app_cpp.insert(std::make_pair(APP_NAME_TMPL, object->GetName()));
    replacement_app_cpp.insert(std::make_pair(APP_DESC_TMPL, ""));
    loadAndReplace(replacement_app_cpp, app_cpp_data);
    saveInFile(m_folderPath + "/" + object->GetName() + ".cpp", app_cpp_data);
    return app_cpp_data;
}

std::string AppGenerator::GenerateProto(TIObject* object, const std::string& parameter)
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
    
    Generator* generator = GeneratorManager::GetInstance().GetGenerator(TIObject::Module);
    if(generator) {
        generator->GenerateProto(object, APP_TMPL);
    }
    return "";
}

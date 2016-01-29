#include "til_generator.h"
#include "resource.h"
#include "common/file.h"

#include <string.h>
#include <algorithm>

#define HEADER_TMPL "##Header##"
#define CLASS_NAME_TMPL "##ClassName##"
#define MODULE_HEADERS_TMPL "##ModulesHeaders##"
#define CHECK_SERVER_TMPL "##CheckServer##"
#define CHECK_MODULES_TMPL "##CheckModules##"
#define CREATE_MODULES_TMPL "##CreateModules##"
#define APP_NAME_TMPL "##AppName##"
#define APP_DESC_TMPL "##AppDescription##"

TILGenerator::TILGenerator(const std::string& folderPath)
: m_folderPath(folderPath)
{
}

TILGenerator::~TILGenerator()
{
}

void TILGenerator::Generate(TIObject* object)
{
    std::vector<TIObject*> childs = object->GetChilds();
    for(std::vector<TIObject*>::iterator it = childs.begin();
        it != childs.end(); it++)
    {
        if((*it)->GetType() == TIObject::Application)
        {
            generateApp(*it);
        }
    }
}

void TILGenerator::generateApp(TIObject* object)
{
    std::vector<std::string> modulesNames;
    std::vector<TIObject*> childs = object->GetChilds();
    for(std::vector<TIObject*>::iterator it = childs.begin();
        it != childs.end(); it++)
    {
        if((*it)->GetType() == TIObject::Module)
        {
            modulesNames.push_back((*it)->GetName());
            //TODO:generate module
        }
    }
    
    //generate app header file
    LOAD_RESOURCE(apps_tilc_resources_application_h_tmpl, app_h_str);
    std::string app_h_data(app_h_str.data(), app_h_str.size());
    std::string header = object->GetName();
    header.append("_H");
    std::transform(header.begin(), header.end(), header.begin(), ::toupper);
    std::map<std::string, std::string> replacement_app_h;
    replacement_app_h.insert(std::make_pair(HEADER_TMPL, header));
    replacement_app_h.insert(std::make_pair(CLASS_NAME_TMPL, object->GetName()));
    loadAndReplace(replacement_app_h, app_h_data);    
    saveInFile(m_folderPath + "/" + object->GetName() + ".h", app_h_data);
    
    //generate app source file
    LOAD_RESOURCE(apps_tilc_resources_application_cpp_tmpl, app_cpp_str);
    std::string app_cpp_data(app_cpp_str.data(), app_cpp_str.size());
    std::string modules_headers, check_modules, create_modules;
    for(std::vector<std::string>::iterator it = modulesNames.begin();
        it != modulesNames.end(); it++)
    {
        modules_headers.append("#include \"");
        modules_headers.append(*it);
        modules_headers.append(".h\"\n");
        check_modules.append("\tif(strcmp(Twainet::GetModuleName(module).m_name, \"");
        check_modules.append(*it);
        check_modules.append("\") == 0)\n\t{\n\t\t");
        check_modules.append(object->GetName());
        check_modules.append("::GetInstance().Stop();\n\t}\n");
        create_modules.append("\tm_modules.push_back(new ");
        create_modules.append(*it);
        create_modules.append("());\n");
    }
    header = object->GetName();
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
    
}

void TILGenerator::loadAndReplace(const std::map<std::string, std::string>& replacement, std::string& data)
{
    for(std::map<std::string, std::string>::const_iterator it = replacement.begin();
        it != replacement.end(); it++)
    {
        size_t pos = 0;
        while(pos != -1)
        {
            pos = data.find(it->first);
            if(pos != -1)
            {
                data.erase(pos, it->first.size());
                data.insert(pos, it->second);
                continue;
            }
        }
    }
}

void TILGenerator::saveInFile(const std::string& filePath, const std::string& data)
{
    File file(filePath);
    file.CreateDir();
    file.Open("wt");
    file.Write(data.c_str(), data.size());
}
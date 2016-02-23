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
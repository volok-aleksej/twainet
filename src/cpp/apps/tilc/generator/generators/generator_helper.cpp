#include "generator_helper.h"
#include "common/file.h"
#include "../interfaces.h"

#include <algorithm>

std::string GeneratorHelper::getDefinedModuleName(const std::string& name)
{
    std::string moduleName(name);
    moduleName.append("_MODULE_NAME");
    std::transform(moduleName.begin(), moduleName.end(), moduleName.begin(), ::toupper);
    return moduleName;
}

std::string GeneratorHelper::getIfnDefHeader(const std::string& name)
{
    std::string header(name);
    header.append("_H");
    std::transform(header.begin(), header.end(), header.begin(), ::toupper);
    return header;
}

void GeneratorHelper::loadAndReplace(const std::map<std::string, std::string>& replacement, std::string& data)
{
    for(std::map<std::string, std::string>::const_iterator it = replacement.begin();
        it != replacement.end(); it++)
    {
        size_t pos = 0;
        while(pos != std::string::npos)
        {
            pos = data.find(it->first);
            if(pos != std::string::npos)
            {
                data.erase(pos, it->first.size());
                data.insert(pos, it->second);
                continue;
            }
        }
    }
}

void GeneratorHelper::saveInFile(const std::string& filePath, const std::string& data)
{
    File file(filePath);
    file.CreateDir();
    file.Open("wt");
    file.Write(data.c_str(), data.size());
}

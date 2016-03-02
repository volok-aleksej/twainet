#ifndef GENERATOR_HELPER_H
#define GENERATOR_HELPER_H

#include <string>
#include <map>

#define MODULE_SERVER_POSTFIX          "ModuleStub"
#define MODULE_CLIENT_POSTFIX          "ModuleProxy"

class GeneratorHelper
{
public:
    std::string getDefinedModuleName(const std::string& name);
    std::string getIfnDefHeader(const std::string& name);
    void loadAndReplace(const std::map<std::string, std::string>& replacement, std::string& data);
    void saveInFile(const std::string& filePath, const std::string& data);
};

#endif/*GENERATOR_HELPER_H*/
#ifndef TIL_GENERATOR_H
#define TIL_GENERATOR_H

#include "compiler/ti_objects.h"
#include <map>

class TILGenerator
{
public:
    TILGenerator(const std::string& folderPath);
    ~TILGenerator();
    
    void Generate(TIObject* object);
protected:
    void generateApp(TIObject* object);
    void loadAndReplace(const std::map<std::string, std::string>& replacement, std::string& data);
    void saveInFile(const std::string& filePath, const std::string& data);
private:
    std::string m_folderPath;
};

#endif/*TIL_GENERATOR_H*/
#ifndef MODULE_GENERATOR_H
#define MODULE_GENERATOR_H

#include "generator_helper.h"
#include "../interfaces.h"
#include "compiler/ti_objects.h"

class ModuleGenerator : public Generator, public GeneratorHelper
{
public:
    ModuleGenerator(const std::string& path);
    virtual ~ModuleGenerator();
    
    virtual std::string GenerateH(TIObject* object);
    virtual std::string GenerateCPP(TIObject* object);
private:
    std::string m_folderPath;
};

#endif/*MODULE_GENERATOR_H*/
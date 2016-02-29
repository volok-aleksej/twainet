#ifndef APP_GENERATOR_H
#define APP_GENERATOR_H

#include "generator_helper.h"
#include "../interfaces.h"
#include "compiler/ti_objects.h"

class AppGenerator : public Generator, public GeneratorHelper
{
public:
    AppGenerator(const std::string& path);
    virtual ~AppGenerator();
    
    virtual std::string GenerateH(TIObject* object);
    virtual std::string GenerateCPP(TIObject* object);
    virtual std::string GenerateProto(TIObject* object);
private:
    std::string m_folderPath;
};

#endif/*APP_GENERATOR_H*/
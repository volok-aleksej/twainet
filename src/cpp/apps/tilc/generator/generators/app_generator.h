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
    
    virtual std::string GenerateH(TIObject* object, const std::string& parameter);
    virtual std::string GenerateCPP(TIObject* object, const std::string& parameter);
    virtual std::string GenerateProto(TIObject* object, const std::string& parameter);
private:
    std::string m_folderPath;
};

#endif/*APP_GENERATOR_H*/
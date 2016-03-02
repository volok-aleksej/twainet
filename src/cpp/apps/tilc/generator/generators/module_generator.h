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
    
    virtual std::string GenerateH(TIObject* object, const std::string& parameter);
    virtual std::string GenerateCPP(TIObject* object, const std::string& parameter);
    virtual std::string GenerateProto(TIObject* object, const std::string& parameter);
protected:
    void generateStubCPP(TIObject* object, const std::string& parameter);
    void generateStubH(TIObject* object, const std::string& parameter);
    void generateProxyCPP(TIObject* object, const std::string& parameter);
    void generateProxyH(TIObject* object, const std::string& parameter);
private:
    std::string m_folderPath;
};

#endif/*MODULE_GENERATOR_H*/
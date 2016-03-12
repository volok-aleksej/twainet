#ifndef FUNCTION_GENERATOR_H
#define FUNCTION_GENERATOR_H

#include "generator_helper.h"
#include "../interfaces.h"
#include "compiler/ti_objects.h"

class FunctionGenerator : public Generator, public GeneratorHelper
{
public:
    FunctionGenerator();
    virtual ~FunctionGenerator();
    
    virtual std::string GenerateH(TIObject* object, const std::string& parameter);
    virtual std::string GenerateCPP(TIObject* object, const std::string& parameter);
    virtual std::string GenerateProto(TIObject* object, const std::string& parameter);
private:
    std::map<std::string, Helper*> m_helpers;
};

#endif/*FUNCTION_GENERATOR_H*/
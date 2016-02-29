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
    
    virtual std::string GenerateH(TIObject* object);
    virtual std::string GenerateCPP(TIObject* object);
};

#endif/*FUNCTION_GENERATOR_H*/
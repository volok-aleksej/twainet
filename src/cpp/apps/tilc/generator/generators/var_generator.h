#ifndef VAR_GENERATOR_H
#define VAR_GENERATOR_H

#include "generator_helper.h"
#include "../interfaces.h"
#include "compiler/ti_objects.h"

class VarGenerator : public Generator, public GeneratorHelper
{
public:
    VarGenerator();
    virtual ~VarGenerator();
    
    virtual std::string GenerateH(TIObject* object, const std::string& parameter);
    virtual std::string GenerateCPP(TIObject* object, const std::string& parameter);
    virtual std::string GenerateProto(TIObject* object, const std::string& parameter);
};

#endif/*VAR_GENERATOR_H*/
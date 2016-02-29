#ifndef TIL_GENERATOR_H
#define TIL_GENERATOR_H

#include "../interfaces.h"
#include "generator_helper.h"
#include <map>

class TILGenerator : public Generator, public GeneratorHelper
{
public:
    TILGenerator();
    virtual ~TILGenerator();
    
    virtual std::string GenerateH(TIObject* object, const std::string& parameter);
    virtual std::string GenerateCPP(TIObject* object, const std::string& parameter);
    virtual std::string GenerateProto(TIObject* object, const std::string& parameter);
};

#endif/*TIL_GENERATOR_H*/
#ifndef FUNCTION_PROXY_H
#define FUNCTION_PROXY_H

#include "../../interfaces.h"
#include "../generator_helper.h"

class FunctionProxy : public Helper, public GeneratorHelper
{
public:
    FunctionProxy();
    ~FunctionProxy();
    
    virtual std::string GenerateH(TIObject* object);
    virtual std::string GenerateCPP(TIObject* object);
    virtual std::string GenerateProto(TIObject* object);
};

#endif/*FUNCTION_PROXY_H*/
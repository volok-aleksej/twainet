#ifndef FUNCTION_APP_H
#define FUNCTION_APP_H

#include "../../interfaces.h"
#include "../generator_helper.h"

class FunctionApp : public Helper, public GeneratorHelper
{
public:
    FunctionApp();
    ~FunctionApp();
    
    virtual std::string GenerateH(TIObject* object);
    virtual std::string GenerateCPP(TIObject* object);
    virtual std::string GenerateProto(TIObject* object);
};

#endif/*FUNCTION_APP_H*/
#ifndef FUNCTION_STUB_H
#define FUNCTION_STUB_H

#include "../../interfaces.h"
#include "../generator_helper.h"

class FunctionStub : public Helper, public GeneratorHelper
{
public:
    FunctionStub();
    ~FunctionStub();
    
    virtual std::string GenerateH(TIObject* object);
    virtual std::string GenerateCPP(TIObject* object);
    virtual std::string GenerateProto(TIObject* object);
};

#endif/*FUNCTION_STUB_H*/
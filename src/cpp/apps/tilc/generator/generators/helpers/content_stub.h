#ifndef CONTENT_STUB_H
#define CONTENT_STUB_H

#include "../../interfaces.h"
#include "../generator_helper.h"

class ContentStub : public Helper, public GeneratorHelper
{
public:
    ContentStub();
    ~ContentStub();
    
    virtual std::string GenerateH(TIObject* object);
    virtual std::string GenerateCPP(TIObject* object);
    virtual std::string GenerateProto(TIObject* object);
};

#endif/*CONTENT_STUB_H*/
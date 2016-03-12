#ifndef CONTENT_PROXY_H
#define CONTENT_PROXY_H

#include "../../interfaces.h"
#include "../generator_helper.h"

class ContentProxy : public Helper, public GeneratorHelper
{
public:
    ContentProxy();
    ~ContentProxy();
    
    virtual std::string GenerateH(TIObject* object);
    virtual std::string GenerateCPP(TIObject* object);
    virtual std::string GenerateProto(TIObject* object);
};

#endif/*CONTENT_PROXY_H*/
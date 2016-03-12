#ifndef ADD_MESSAGE_H
#define ADD_MESSAGE_H

#include "../../interfaces.h"
#include "../generator_helper.h"

class AddMessageStub : public Helper, public GeneratorHelper
{
public:
    AddMessageStub();
    ~AddMessageStub();
    
    virtual std::string GenerateH(TIObject* object);
    virtual std::string GenerateCPP(TIObject* object);
    virtual std::string GenerateProto(TIObject* object);
};

class AddMessageProxy : public Helper, public GeneratorHelper
{
public:
    AddMessageProxy();
    ~AddMessageProxy();
    
    virtual std::string GenerateH(TIObject* object);
    virtual std::string GenerateCPP(TIObject* object);
    virtual std::string GenerateProto(TIObject* object);
};

#endif/*ADD_MESSAGE_H*/
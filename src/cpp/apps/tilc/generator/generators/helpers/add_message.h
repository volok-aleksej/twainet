#ifndef ADD_MESSAGE_H
#define ADD_MESSAGE_H

#include "../../interfaces.h"
#include "../generator_helper.h"

class AddMessage : public Helper, public GeneratorHelper
{
public:
    AddMessage();
    ~AddMessage();
    
    virtual std::string GenerateH(TIObject* object);
    virtual std::string GenerateCPP(TIObject* object);
    virtual std::string GenerateProto(TIObject* object);
};

#endif/*ADD_MESSAGE_H*/
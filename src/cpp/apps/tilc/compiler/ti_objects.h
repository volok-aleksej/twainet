#ifndef TI_OBJECTS_H
#define TI_OBJECTS_H

#include "ti_object.h"

class ContainerObject : public TIObject
{
public:
    enum Type
    {
        Plugin = (int)TIObject::Plugin,
        Module,
        Application,
    };

    ContainerObject(Type type, const std::string& name)
    : TIObject((TIObject::typeObject)type, name){}
    virtual ~ContainerObject (){}
};

class RetObject : public TIObject
{    
public:
    enum Type
    {
        Variable = (int)TIObject::Variable,
        Function
    };
    
    RetObject(Type type, const std::string& name, const std::string& retVal)
    : TIObject((TIObject::typeObject)type, name), m_retVal(retVal){}
    
    std::string GetRetValue()
    {
        return m_retVal;
    }
    
private:
    std::string m_retVal;
};

#endif/*TI_OBJECTS_H*/
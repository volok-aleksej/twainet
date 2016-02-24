#ifndef GENERATOR_MANAGER_H
#define GENERATOR_MANAGER_H

#include "include/singleton.h"
#include "compiler/ti_objects.h"
#include "interfaces.h"
#include <map>

class GeneratorManager : public Singleton<GeneratorManager>
{
    friend class Singleton<GeneratorManager>;
    GeneratorManager();
public:
    ~GeneratorManager();
    
    void Init(const std::string& path);
    Generator* GetGenerator(TIObject::typeObject type);
private:
    std::map<TIObject::typeObject, Generator*> m_generators;
};

#endif/*GENERATOR_MANAGER_H*/
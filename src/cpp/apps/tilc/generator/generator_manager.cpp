#include "generator_manager.h"
#include "generators/function_generator.h"
#include "generators/module_generator.h"
#include "generators/app_generator.h"
#include "generators/til_generator.h"

GeneratorManager::GeneratorManager()
{
}

GeneratorManager::~GeneratorManager()
{
    for(std::map<TIObject::typeObject, Generator*>::iterator it = m_generators.begin();
        it != m_generators.end(); it++)
    {
        delete it->second;
    }
}

void GeneratorManager::Init(const std::string& path)
{
    m_generators.insert(std::make_pair(TIObject::Root, new TILGenerator(path)));
    m_generators.insert(std::make_pair(TIObject::Application, new AppGenerator(path)));
    m_generators.insert(std::make_pair(TIObject::Module, new ModuleGenerator(path)));
    m_generators.insert(std::make_pair(TIObject::Function, new FunctionGenerator()));
}

Generator* GeneratorManager::GetGenerator(TIObject::typeObject type)
{
    if(m_generators.find(type) != m_generators.end())
    {
        return m_generators[type];
    }
    
    return 0;
}
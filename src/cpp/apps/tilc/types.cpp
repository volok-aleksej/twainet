#include "types.h"

bool TypesManager::IsInType(const std::string& type)
{
    return m_c_types.find(type) != m_c_types.end();
}
    
std::string TypesManager::GetCType(const std::string& type)
{
    return m_c_types[type];
}
    
std::map<std::string, std::string> TypesManager::GenerateTypes()
{
    std::map<std::string, std::string> types;
    types.insert(std::make_pair("int", "int"));
    types.insert(std::make_pair("bool", "bool"));
    types.insert(std::make_pair("float", "float"));
    types.insert(std::make_pair("void", "void"));
    types.insert(std::make_pair("short", "short"));
    types.insert(std::make_pair("char", "char"));
    types.insert(std::make_pair("string", "std::string"));
    return types;
}

std::map<std::string, std::string> TypesManager::m_c_types = TypesManager::GenerateTypes();
#include "types.h"

bool TypesManager::IsInType(const std::string& type)
{
    return m_c_types.find(type) != m_c_types.end();
}
    
std::string TypesManager::GetCType(const std::string& type)
{
    return m_c_types[type];
}

std::string TypesManager::GetProtoType(const std::string& type)
{
    return m_proto_types[type];
}

std::string TypesManager::GetDefaultReturn(const std::string& type)
{
    return m_default_types[type];
}

std::map<std::string, std::string> TypesManager::GenerateCTypes()
{
    std::map<std::string, std::string> types;
    types.insert(std::make_pair("int", "int"));
    types.insert(std::make_pair("bool", "bool"));
    types.insert(std::make_pair("float", "float"));
    types.insert(std::make_pair("void", "void"));
    types.insert(std::make_pair("short", "short"));
    types.insert(std::make_pair("char", "char"));
    types.insert(std::make_pair("string", "const std::string&"));
    return types;
}

std::map<std::string, std::string> TypesManager::GenerateProtoTypes()
{
    std::map<std::string, std::string> types;
    types.insert(std::make_pair("int", "int32"));
    types.insert(std::make_pair("bool", "bool"));
    types.insert(std::make_pair("float", "float"));
    types.insert(std::make_pair("void", ""));
    types.insert(std::make_pair("short", "int32"));
    types.insert(std::make_pair("char", "uint32"));
    types.insert(std::make_pair("string", "string"));
    return types;
}

std::map<std::string, std::string> TypesManager::GenerateDefaultTypes()
{
    std::map<std::string, std::string> types;
    types.insert(std::make_pair("int", "0"));
    types.insert(std::make_pair("bool", "false"));
    types.insert(std::make_pair("float", "0"));
    types.insert(std::make_pair("void", ""));
    types.insert(std::make_pair("short", "0"));
    types.insert(std::make_pair("char", "0"));
    types.insert(std::make_pair("string", "\"\""));
    return types;
}


std::map<std::string, std::string> TypesManager::m_default_types = TypesManager::GenerateDefaultTypes();
std::map<std::string, std::string> TypesManager::m_c_types = TypesManager::GenerateCTypes();
std::map<std::string, std::string> TypesManager::m_proto_types = TypesManager::GenerateProtoTypes();

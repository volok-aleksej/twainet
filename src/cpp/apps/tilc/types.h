#ifndef TYPES_H
#define TYPES_H

#include <map>
#include <string>

class TypesManager
{
public:
    static bool IsInType(const std::string& type);
    static std::string GetCType(const std::string& type);
    static std::string GetProtoType(const std::string& type);
    static std::string GetDefaultReturn(const std::string& type);
private:
    static std::map<std::string, std::string> GenerateCTypes();
    static std::map<std::string, std::string> GenerateProtoTypes();
    static std::map<std::string, std::string> GenerateDefaultTypes();
private:
    static std::map<std::string, std::string> m_c_types;
    static std::map<std::string, std::string> m_default_types;
    static std::map<std::string, std::string> m_proto_types;
};

#endif/*TYPES_H*/
#include "module_generator.h"
#include "../generator_manager.h"
#include "resource.h"
#include <algorithm>

ModuleGenerator::ModuleGenerator(const std::string& path)
: m_folderPath(path)
{
}

ModuleGenerator::~ModuleGenerator()
{
}
    
std::string ModuleGenerator::GenerateH(TIObject* object)
{
    std::string nameLower(object->GetName());
    std::transform(nameLower.begin(), nameLower.end(), nameLower.begin(), ::tolower);
    std::string content;
    std::vector<TIObject*> childs = object->GetChilds();
    for(std::vector<TIObject*>::iterator it = childs.begin();
        it != childs.end(); it++)
    {
        Generator* generator = GeneratorManager::GetInstance().GetGenerator((*it)->GetType());
        if(generator)
        {
            content.append(generator->GenerateH(*it));
        }
    }

    LOAD_RESOURCE(apps_tilc_resources_module_h_tmpl, module_h_str);
    std::string module_h_data(module_h_str.data(), module_h_str.size());
    std::string defines("#define ");
    defines.append(getDefinedModuleName(object->GetName()));
    defines.append(" \"");
    defines.append(object->GetName());
    defines.append("\"");
    std::string include("#include <string>\n");
    include.append("#pragma warning(disable:4244 4267)\n");
    include.append("#include \"messages/");
    include.append(nameLower);
    include.append(".pb.h\"\n");
    include.append("using namespace ");
    include.append(nameLower);
    include.append(";\n");
    include.append("#pragma warning(default:4244 4267)\n");
    std::map<std::string, std::string> replacement_module_h;
    replacement_module_h.insert(std::make_pair(HEADER_TMPL, getIfnDefHeader(object->GetName())));
    replacement_module_h.insert(std::make_pair(INCLUDES_TMPL, include));
    replacement_module_h.insert(std::make_pair(DEFINES_TMPL, defines));
    replacement_module_h.insert(std::make_pair(CLASS_NAME_TMPL, object->GetName() + "StubImpl"));
    replacement_module_h.insert(std::make_pair(CONTENT_DECLARE_TMPL, content));
    loadAndReplace(replacement_module_h, module_h_data);
    saveInFile(m_folderPath + "/" + object->GetName() + "StubImpl.h", module_h_data);
    return module_h_data;
}

std::string ModuleGenerator::GenerateCPP(TIObject* object)
{
    return "";
}

std::string ModuleGenerator::GenerateProto(TIObject* object)
{
    std::string nameLower(object->GetName());
    std::transform(nameLower.begin(), nameLower.end(), nameLower.begin(), ::tolower);
    std::string messages;
    std::vector<TIObject*> childs = object->GetChilds();
    for(std::vector<TIObject*>::iterator it = childs.begin();
        it != childs.end(); it++)
    {
        Generator* generator = GeneratorManager::GetInstance().GetGenerator((*it)->GetType());
        if(generator)
        {
            messages.append(generator->GenerateProto(*it));
        }
    }
    
    
    LOAD_RESOURCE(apps_tilc_resources_module_proto_tmpl, module_proto_str);
    std::string module_proto_data(module_proto_str.data(), module_proto_str.size());
    std::map<std::string, std::string> replacement_module_proto;
    replacement_module_proto.insert(std::make_pair(PACKAGE_NAME_TMPL, nameLower));
    replacement_module_proto.insert(std::make_pair(MESSAGES_TMPL, messages));
    replacement_module_proto.insert(std::make_pair(ENUMS_TMPL, ""));
    loadAndReplace(replacement_module_proto, module_proto_data);
    saveInFile(m_folderPath + "/" + nameLower + ".proto", module_proto_data);
    return module_proto_data;
}

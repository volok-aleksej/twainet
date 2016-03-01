#include "module_generator.h"
#include "../generator_manager.h"
#include "resource.h"
#include "types.h"
#include <algorithm>

ModuleGenerator::ModuleGenerator(const std::string& path)
: m_folderPath(path)
{
}

ModuleGenerator::~ModuleGenerator()
{
}
    
std::string ModuleGenerator::GenerateH(TIObject* object, const std::string& parameter)
{
    std::string nameLower(object->GetName());
    std::transform(nameLower.begin(), nameLower.end(), nameLower.begin(), ::tolower);
    std::string content;
    std::string functions;
    std::vector<TIObject*> childs = object->GetChilds();
    for(std::vector<TIObject*>::iterator it = childs.begin();
        it != childs.end(); it++)
    {
        if((*it)->GetType() == TIObject::Variable ||
           (*it)->GetType() == TIObject::Function)
        {
            Generator* generator = GeneratorManager::GetInstance().GetGenerator((*it)->GetType());
            if(generator)
            {
                content.append(generator->GenerateH(*it, CONTENT_DECLARE_TMPL));
                functions.append(generator->GenerateH(*it, FUNCTIONS_TMPL));
            }
        }
    }

    LOAD_RESOURCE(apps_tilc_resources_module_h_tmpl, module_h_str);
    std::string module_h_data(module_h_str.data(), module_h_str.size());
    std::string defines("#define ");
    defines.append(getDefinedModuleName(object->GetName()));
    defines.append(" \"");
    defines.append(object->GetName());
    defines.append("\"");
    if(parameter == APP_TMPL)
    {
        defines.append("\nclass ");
        defines.append(object->GetName());
        defines.append(";");
    }
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
    replacement_module_h.insert(std::make_pair(HEADER_TMPL, getIfnDefHeader(object->GetName() + MODULE_POSTFIX)));
    replacement_module_h.insert(std::make_pair(INCLUDES_TMPL, include));
    replacement_module_h.insert(std::make_pair(DEFINES_TMPL, defines));
    replacement_module_h.insert(std::make_pair(CLASS_NAME_TMPL, object->GetName() + MODULE_POSTFIX));
    replacement_module_h.insert(std::make_pair(CONTENT_DECLARE_TMPL, content));
    replacement_module_h.insert(std::make_pair(FUNCTIONS_TMPL, functions));
    if(parameter == APP_TMPL)
    {
        std::string app(object->GetName() + "* ");
        replacement_module_h.insert(std::make_pair(APP_TMPL, app + "app"));
        replacement_module_h.insert(std::make_pair(APP_MEMBER_TMPL, app + "m_app;"));
    }
    else
    {
        replacement_module_h.insert(std::make_pair(APP_TMPL, ""));
        replacement_module_h.insert(std::make_pair(APP_MEMBER_TMPL, ""));
    }
    loadAndReplace(replacement_module_h, module_h_data);
    saveInFile(m_folderPath + "/" + object->GetName() + MODULE_POSTFIX + ".h", module_h_data);
    return module_h_data;
}

std::string ModuleGenerator::GenerateCPP(TIObject* object, const std::string& parameter)
{
    std::string content;
    std::string functions;
    std::string addMessages;
    std::vector<TIObject*> childs = object->GetChilds();
    for(std::vector<TIObject*>::iterator it = childs.begin();
        it != childs.end(); it++)
    {
        if((*it)->GetType() == TIObject::Variable ||
           (*it)->GetType() == TIObject::Function)
        {
            Generator* generator = GeneratorManager::GetInstance().GetGenerator((*it)->GetType());
            if(generator)
            {
                content.append(generator->GenerateCPP(*it, CONTENT_DECLARE_TMPL));
                functions.append(generator->GenerateCPP(*it, FUNCTIONS_TMPL));
                addMessages.append(generator->GenerateCPP(*it, ADD_MESSAGES_TMPL));
                std::map<std::string, std::string> replacement_functions;
                if(parameter == APP_TMPL)
                {
                    std::string fapp = generator->GenerateCPP(*it, FUNCTIONS_APP_TMPL);
                    replacement_functions.insert(std::make_pair(FUNCTIONS_APP_TMPL, fapp));
                }
                else
                {
                    RetObject* retObject = dynamic_cast<RetObject*>(*it);
                    if(retObject)
                    {
                        std::string returnVal = TypesManager::GetDefaultReturn(retObject->GetRetValue());
                        std::string fapp("// not implemented by default\n");
                        if(!returnVal.empty())
                        {
                            fapp.append("    return ");
                            fapp.append(returnVal);
                            fapp.append(";\n");
                        }
                        replacement_functions.insert(std::make_pair(FUNCTIONS_APP_TMPL, fapp));
                    }
                }
                loadAndReplace(replacement_functions, functions);
            }
        }
    }
    
    LOAD_RESOURCE(apps_tilc_resources_module_cpp_tmpl, module_cpp_str);
    std::string module_cpp_data(module_cpp_str.data(), module_cpp_str.size());
    std::map<std::string, std::string> replacement_module_cpp;
    replacement_module_cpp.insert(std::make_pair(HEADER_TMPL, object->GetName() + MODULE_POSTFIX + ".h"));
    replacement_module_cpp.insert(std::make_pair(CLASS_NAME_TMPL, object->GetName() + MODULE_POSTFIX));
    replacement_module_cpp.insert(std::make_pair(CONTENT_DECLARE_TMPL, content));
    replacement_module_cpp.insert(std::make_pair(FUNCTIONS_TMPL, functions));
    replacement_module_cpp.insert(std::make_pair(ADD_MESSAGES_TMPL, addMessages));
    replacement_module_cpp.insert(std::make_pair(DEFINES_TMPL, getDefinedModuleName(object->GetName())));
    if(parameter == APP_TMPL)
    {
        std::string app(object->GetName() + "* ");
        replacement_module_cpp.insert(std::make_pair(APP_TMPL, app + "app"));
        replacement_module_cpp.insert(std::make_pair(APP_MEMBER_TMPL, ", m_app(app)\n"));
        std::string includes("#include \"");
        includes.append(object->GetName());
        includes.append(".h\"\n");
        replacement_module_cpp.insert(std::make_pair(INCLUDES_TMPL, includes));
    }
    else
    {
        replacement_module_cpp.insert(std::make_pair(APP_TMPL, ""));
        replacement_module_cpp.insert(std::make_pair(APP_MEMBER_TMPL, ""));
        replacement_module_cpp.insert(std::make_pair(INCLUDES_TMPL, ""));
    }
    loadAndReplace(replacement_module_cpp, module_cpp_data);
    saveInFile(m_folderPath + "/" + object->GetName() + MODULE_POSTFIX + ".cpp", module_cpp_data);
    return "";
}

std::string ModuleGenerator::GenerateProto(TIObject* object, const std::string& parameter)
{
    std::string nameLower(object->GetName());
    std::transform(nameLower.begin(), nameLower.end(), nameLower.begin(), ::tolower);
    std::string messages;
    std::vector<TIObject*> childs = object->GetChilds();
    for(std::vector<TIObject*>::iterator it = childs.begin();
        it != childs.end(); it++)
    {
        if((*it)->GetType() == TIObject::Variable ||
           (*it)->GetType() == TIObject::Function)
        {
            Generator* generator = GeneratorManager::GetInstance().GetGenerator((*it)->GetType());
            if(generator)
            {
                messages.append(generator->GenerateProto(*it));
            }
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

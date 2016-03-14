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
    generateStubH(object, parameter);
    generateProxyH(object, parameter);
    return "";
}

std::string ModuleGenerator::GenerateCPP(TIObject* object, const std::string& parameter)
{
    generateStubCPP(object, parameter);
    generateProxyCPP(object, parameter);
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

void ModuleGenerator::generateStubH(TIObject* object, const std::string& parameter)
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
                content.append(generator->GenerateH(*it, CONTENT_DECLARE_STUB_TMPL));
                functions.append(generator->GenerateH(*it, FUNCTIONS_STUB_TMPL));
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
    if(parameter == ARGS_TMPL)
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
    std::string functionsProxy("    friend class ");
    functionsProxy.append(object->GetName() + MODULE_CLIENT_POSTFIX);
    functionsProxy.append(";");
    std::map<std::string, std::string> replacement_module_h;
    replacement_module_h.insert(std::make_pair(HEADER_TMPL, getIfnDefHeader(object->GetName() + MODULE_SERVER_POSTFIX)));
    replacement_module_h.insert(std::make_pair(INCLUDES_TMPL, include));
    replacement_module_h.insert(std::make_pair(DEFINES_TMPL, defines));
    replacement_module_h.insert(std::make_pair(CLASS_NAME_TMPL, object->GetName() + MODULE_SERVER_POSTFIX));
    replacement_module_h.insert(std::make_pair(CONTENT_DECLARE_STUB_TMPL, content));
    replacement_module_h.insert(std::make_pair(FUNCTIONS_STUB_TMPL, functions));
    replacement_module_h.insert(std::make_pair(CONTENT_DECLARE_PROXY_TMPL, ""));
    replacement_module_h.insert(std::make_pair(FUNCTIONS_PROXY_TMPL, functionsProxy));
    replacement_module_h.insert(std::make_pair(INHERITE_TMPL, ": public Module"));
    if(parameter == ARGS_TMPL)
    {
        std::string app(object->GetName() + "* ");
        replacement_module_h.insert(std::make_pair(ARGS_TMPL, app + "app"));
        replacement_module_h.insert(std::make_pair(ARGS_MEMBER_TMPL, app + "m_app;"));
    }
    else
    {
        replacement_module_h.insert(std::make_pair(ARGS_TMPL, ""));
        replacement_module_h.insert(std::make_pair(ARGS_MEMBER_TMPL, ""));
    }
    loadAndReplace(replacement_module_h, module_h_data);
    saveInFile(m_folderPath + "/" + object->GetName() + MODULE_SERVER_POSTFIX + ".h", module_h_data);
}

void ModuleGenerator::generateStubCPP(TIObject* object, const std::string& parameter)
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
                content.append(generator->GenerateCPP(*it, CONTENT_DECLARE_STUB_TMPL));
                functions.append(generator->GenerateCPP(*it, FUNCTIONS_STUB_TMPL));
                addMessages.append(generator->GenerateCPP(*it, ADD_MESSAGES_STUB_TMPL));
                std::map<std::string, std::string> replacement_functions;
                if(parameter == ARGS_TMPL)
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
    replacement_module_cpp.insert(std::make_pair(HEADER_TMPL, object->GetName() + MODULE_SERVER_POSTFIX + ".h"));
    replacement_module_cpp.insert(std::make_pair(CLASS_NAME_TMPL, object->GetName() + MODULE_SERVER_POSTFIX));
    replacement_module_cpp.insert(std::make_pair(CONTENT_DECLARE_STUB_TMPL, content));
    replacement_module_cpp.insert(std::make_pair(FUNCTIONS_STUB_TMPL, functions));
    replacement_module_cpp.insert(std::make_pair(CONTENT_DECLARE_PROXY_TMPL, ""));
    replacement_module_cpp.insert(std::make_pair(FUNCTIONS_PROXY_TMPL, ""));
    replacement_module_cpp.insert(std::make_pair(ADD_MESSAGES_TMPL, addMessages));
    std::string includes("#include \"");
    includes.append(object->GetName() + MODULE_CLIENT_POSTFIX + ".h\"\n");
    std::string appModuleTmpl(": Module(");
    appModuleTmpl.append(getDefinedModuleName(object->GetName()));
    appModuleTmpl.append(")");
    std::string app;
    if(parameter == ARGS_TMPL)
    {
        app.append(object->GetName() + "* app");
        appModuleTmpl.append(", m_app(app)\n");
        includes.append("#include \"");
        includes.append(object->GetName());
        includes.append(".h\"\n");
    }
    replacement_module_cpp.insert(std::make_pair(ARGS_TMPL, app));
    replacement_module_cpp.insert(std::make_pair(ARGS_MEMBER_TMPL, appModuleTmpl));
    replacement_module_cpp.insert(std::make_pair(INCLUDES_TMPL, includes));
    loadAndReplace(replacement_module_cpp, module_cpp_data);
    saveInFile(m_folderPath + "/" + object->GetName() + MODULE_SERVER_POSTFIX + ".cpp", module_cpp_data);
}

void ModuleGenerator::generateProxyCPP(TIObject* object, const std::string& parameter)
{
    std::string nameLower(object->GetName());
    std::transform(nameLower.begin(), nameLower.end(), nameLower.begin(), ::tolower);
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
                addMessages.append(generator->GenerateCPP(*it, ADD_MESSAGES_PROXY_TMPL));
                content.append(generator->GenerateCPP(*it, CONTENT_DECLARE_PROXY_TMPL));
                functions.append(generator->GenerateCPP(*it, FUNCTIONS_PROXY_TMPL));
            }
        }
    }
    
    std::string includes("#include \"");
    includes.append(object->GetName() + MODULE_SERVER_POSTFIX + ".h\"\n");
    LOAD_RESOURCE(apps_tilc_resources_module_cpp_tmpl, module_cpp_str);
    std::string module_cpp_data(module_cpp_str.data(), module_cpp_str.size());
    std::map<std::string, std::string> replacement_module_cpp;
    replacement_module_cpp.insert(std::make_pair(HEADER_TMPL, object->GetName() + MODULE_CLIENT_POSTFIX + ".h"));
    replacement_module_cpp.insert(std::make_pair(CLASS_NAME_TMPL, object->GetName() + MODULE_CLIENT_POSTFIX));
    replacement_module_cpp.insert(std::make_pair(CONTENT_DECLARE_STUB_TMPL, ""));
    replacement_module_cpp.insert(std::make_pair(FUNCTIONS_STUB_TMPL, ""));
    replacement_module_cpp.insert(std::make_pair(CONTENT_DECLARE_PROXY_TMPL, content));
    replacement_module_cpp.insert(std::make_pair(FUNCTIONS_PROXY_TMPL, functions));
    replacement_module_cpp.insert(std::make_pair(ADD_MESSAGES_TMPL, addMessages));
    replacement_module_cpp.insert(std::make_pair(ARGS_TMPL, "IModule* module"));
    replacement_module_cpp.insert(std::make_pair(ARGS_MEMBER_TMPL, ": m_module(module)"));
    replacement_module_cpp.insert(std::make_pair(INCLUDES_TMPL, includes));
    loadAndReplace(replacement_module_cpp, module_cpp_data);
    saveInFile(m_folderPath + "/" + object->GetName() + MODULE_CLIENT_POSTFIX + ".cpp", module_cpp_data);
}

void ModuleGenerator::generateProxyH(TIObject* object, const std::string& parameter)
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
                content.append(generator->GenerateH(*it, CONTENT_DECLARE_PROXY_TMPL));
                functions.append(generator->GenerateH(*it, FUNCTIONS_PROXY_TMPL));
            }
        }
    }

    LOAD_RESOURCE(apps_tilc_resources_module_h_tmpl, module_h_str);
    std::string module_h_data(module_h_str.data(), module_h_str.size());
    std::string include("#include <string>\n");
    include.append("#include <map>\n");
    include.append("#include \"include/semaphore.h\"\n");
    include.append("#pragma warning(disable:4244 4267)\n");
    include.append("#include \"messages/");
    include.append(nameLower);
    include.append(".pb.h\"\n");
    include.append("using namespace ");
    include.append(nameLower);
    include.append(";\n");
    include.append("#pragma warning(default:4244 4267)\n");
    std::string functionsStub("    friend class ");
    functionsStub.append(object->GetName() + MODULE_SERVER_POSTFIX);
    functionsStub.append(";");
    std::string argMember("IModule* m_module;\n");
    argMember.append("std::map<int, Semaphore*> m_requests;\n");
    std::map<std::string, std::string> replacement_module_h;
    replacement_module_h.insert(std::make_pair(HEADER_TMPL, getIfnDefHeader(object->GetName() + MODULE_CLIENT_POSTFIX)));
    replacement_module_h.insert(std::make_pair(INCLUDES_TMPL, include));
    replacement_module_h.insert(std::make_pair(DEFINES_TMPL, ""));
    replacement_module_h.insert(std::make_pair(CLASS_NAME_TMPL, object->GetName() + MODULE_CLIENT_POSTFIX));
    replacement_module_h.insert(std::make_pair(CONTENT_DECLARE_PROXY_TMPL, content));
    replacement_module_h.insert(std::make_pair(FUNCTIONS_PROXY_TMPL, functions));
    replacement_module_h.insert(std::make_pair(CONTENT_DECLARE_STUB_TMPL, ""));
    replacement_module_h.insert(std::make_pair(FUNCTIONS_STUB_TMPL, functionsStub));
    replacement_module_h.insert(std::make_pair(ARGS_TMPL, "IModule* module"));
    replacement_module_h.insert(std::make_pair(ARGS_MEMBER_TMPL, argMember));
    replacement_module_h.insert(std::make_pair(INHERITE_TMPL, ""));
    loadAndReplace(replacement_module_h, module_h_data);
    saveInFile(m_folderPath + "/" + object->GetName() + MODULE_CLIENT_POSTFIX + ".h", module_h_data);
}

    
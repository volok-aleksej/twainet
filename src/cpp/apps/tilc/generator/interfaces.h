#ifndef INTERFACES_H
#define INTERFACES_H

#define ARGS_TMPL "##Args##"
#define ARGS_MEMBER_TMPL "##ArgsMember##"
#define HEADER_TMPL "##Header##"
#define CLASS_NAME_TMPL "##ClassName##"
#define MODULE_HEADERS_TMPL "##ModulesHeaders##"
#define CHECK_SERVER_TMPL "##CheckServer##"
#define CHECK_MODULES_TMPL "##CheckModules##"
#define CREATE_MODULES_TMPL "##CreateModules##"
#define APP_NAME_TMPL "##AppName##"
#define APP_DESC_TMPL "##AppDescription##"
#define INCLUDES_TMPL "##Includes##"
#define DEFINES_TMPL "##Defines##"
#define CONTENT_DECLARE_STUB_TMPL "##ContentDeclareStub##"
#define CONTENT_DECLARE_PROXY_TMPL "##ContentDeclareProxy##"
#define PACKAGE_NAME_TMPL "##PackageName##"
#define ENUMS_TMPL "##Enums##"
#define MESSAGES_TMPL "##Messages##"
#define FUNCTIONS_TMPL "##Functions##"
#define FUNCTIONS_STUB_TMPL "##FunctionsStub##"
#define FUNCTIONS_PROXY_TMPL "##FunctionsProxy##"
#define FUNCTIONS_APP_TMPL "##FunctionsApp##"
#define ADD_MESSAGES_TMPL "##AddMessages##"

#define INHERITE_TMPL "##: public Module##"

#include "compiler/ti_objects.h"

class Generator
{
public:
    virtual ~Generator(){}
    
    virtual std::string GenerateH(TIObject* object, const std::string& parameter = "") = 0;
    virtual std::string GenerateCPP(TIObject* object, const std::string& parameter = "") = 0;
    virtual std::string GenerateProto(TIObject* object, const std::string& parameter = "") = 0;
};

#endif/*INTERFACES_H*/
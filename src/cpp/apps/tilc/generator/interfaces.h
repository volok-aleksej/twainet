#ifndef INTERFACES_H
#define INTERFACES_H

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
#define CONTENT_DECLARE_TMPL "##ContentDeclare##"
#define PACKAGE_NAME_TMPL "##PackageName##"
#define ENUMS_TMPL "##Enums##"
#define MESSAGES_TMPL "##Messages##"

#include "compiler/ti_objects.h"

class Generator
{
public:
    virtual ~Generator(){}
    
    virtual std::string GenerateH(TIObject* object) = 0;
    virtual std::string GenerateCPP(TIObject* object) = 0;
    virtual std::string GenerateProto(TIObject* object) = 0;
};

#endif/*INTERFACES_H*/
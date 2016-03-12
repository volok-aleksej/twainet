#include "add_message.h"
#include "types.h"

AddMessage::AddMessage()
{
}

AddMessage::~AddMessage()
{
}
   
std::string AddMessage::GenerateH(TIObject* object)
{
    return "";
}

std::string AddMessage::GenerateCPP(TIObject* object)
{
    std::string result;
    RetObject* retObject = dynamic_cast<RetObject*>(object);
    if(!object || !retObject)
    {
        return result;
    }

    result.append("    AddMessage(new ");
    result.append(object->GetName());
    result.append("Message(this));\n");
    return result;
}

std::string AddMessage::GenerateProto(TIObject* object)
{
    return "";
}
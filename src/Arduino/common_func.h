#ifndef GUID_GENERATOR_H
#define GUID_GENERATOR_H

#include <WString.h>

String CreateGUID();
String RandString(int size);
int GetError();

#endif/*GUID_GENERATOR_H*/
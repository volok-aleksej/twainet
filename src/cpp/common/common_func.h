#ifndef GUID_GENERATOR_H
#define GUID_GENERATOR_H

#include "common/tstring.h"

std::string CreateGUID();
std::string RandString(int size);
int GetError();

#endif/*GUID_GENERATOR_H*/
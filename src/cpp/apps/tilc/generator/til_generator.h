#ifndef TIL_GENERATOR_H
#define TIL_GENERATOR_H

#include "compiler/ti_objects.h"

class TILGenerator
{
public:
    TILGenerator(const std::string& folderPath);
    ~TILGenerator();
    
    void Generate(TIObject* object);
private:
    std::string m_folderPath;
};

#endif/*TIL_GENERATOR_H*/
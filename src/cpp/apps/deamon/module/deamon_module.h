#ifndef DEAMON_MODULE_H
#define DEAMON_MODULE_H

#include "module.h"

class DeamonModule : public Module
{
public:
	DeamonModule(const Twainet::Module& module);
	virtual ~DeamonModule();
};

#endif/*DEAMON_MODULE_H*/
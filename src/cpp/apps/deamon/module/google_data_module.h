#ifndef GOOGLE_DATA_MODULE_H
#define GOOGLE_DATA_MODULE_H

#include "module.h"
#include "client_module_name.h"
#include "thread_lib/common/object_manager.h"

#pragma warning(disable:4244 4267)
#include "../messages/deamon.pb.h"
using namespace deamon;
#pragma warning(default:4244 4267)

#define GOOGLE_DATA_NAME	"googledata"

class GoogleDataModule : public Module
{
public:
	GoogleDataModule();
	virtual ~GoogleDataModule();
private:
	std::string m_accountName;
};

#endif/*GOOGLE_DATA_MODULE_H*/
#include "google_data_module.h"
#include "../application/config.h"

GoogleDataModule::GoogleDataModule()
: Module(GOOGLE_DATA_NAME, Twainet::IPV4, false)
{
	m_accountName = Config::GetInstance().GetGoogleAccountName();
}

GoogleDataModule::~GoogleDataModule()
{
}

#include "google_data_module.h"

typedef DeamonMessage<LocalServerAttributes, GoogleDataModule> LocalServerAttributesMessage;
typedef DeamonMessage<ClientName, GoogleDataModule> ClientNameMessage;
typedef DeamonMessage<ClientNameList, GoogleDataModule> ClientNameListMessage;

GoogleDataModule::GoogleDataModule(const Twainet::Module& module)
: Module(module)
{
	AddMessage(new LocalServerAttributesMessage(this));
	AddMessage(new ClientNameListMessage(this));
}

GoogleDataModule::~GoogleDataModule()
{
}

void GoogleDataModule::onMessage(const LocalServerAttributes& msg, const Twainet::ModuleName& path)
{
	Twainet::UserPassword userpwd = {0};
	strcpy(userpwd.m_user, msg.username().c_str());
	strcpy(userpwd.m_pass, msg.password().c_str());
	Twainet::ConnectToServer(GetModule(), "localhost", msg.port(), userpwd);
}

void GoogleDataModule::onMessage(const ClientNameList& msg, const Twainet::ModuleName& path)
{
	for(int i = 0; i < msg.name_list_size(); i++)
	{
		ClientModuleName cmname(msg.name_list(i).ipc_name(), msg.name_list(i).host_name());
		m_clientsName.AddObject(cmname);
	}
}

void GoogleDataModule::OnServerConnected(const char* sessionId)
{
	m_sessionId = sessionId;
	Twainet::ModuleName moduleName = {0};
	strcpy(moduleName.m_name, Twainet::ServerModuleName);
	strcpy(moduleName.m_host, sessionId);
	ClientNameMessage cnMsg(this);
	cnMsg.set_ipc_name(GOOGLE_DATA_NAME);
	cnMsg.set_host_name(sessionId);
	toMessage(cnMsg, moduleName);
}

void GoogleDataModule::OnMessageRecv(const Twainet::Message& message)
{
	onData(message.m_typeMessage, message.m_target, (char*)message.m_data, message.m_dataLen);
}
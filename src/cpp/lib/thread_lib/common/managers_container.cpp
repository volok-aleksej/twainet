#include "managers_container.h"
#include "include/ref.h"

/****************************************************************************************************************/
/*                                              DynamicManager                                                  */
/****************************************************************************************************************/
DynamicManager::DynamicManager()
	: m_isStop(false)
{
}

DynamicManager::~DynamicManager()
{
}

void DynamicManager::Stop()
{
	m_isStop = true;
}

bool DynamicManager::IsStop()
{
	return m_isStop;
}

/****************************************************************************************************************/
/*                                             ManagersContainer                                                */
/****************************************************************************************************************/
ManagersContainer::ManagersContainer()
: m_isExit(false)
{
	Start();
}

ManagersContainer::~ManagersContainer()
{
	Stop();
	Join();
}

void ManagersContainer::AddManager(IManager* manager)
{
	manager->ManagerStart();
	m_managers.AddObject(manager);
}

void ManagersContainer::RemoveManager(IManager* manager)
{
	manager->ManagerStop();
	m_managers.RemoveObject(manager);
}

bool ManagersContainer::RunManager(const std::vector<IManager*>& managers, const IManager* manager)
{
	IManager* manager_ = const_cast<IManager*>(manager);
	manager_->ManagerFunc();
	if (manager_->IsStop())
	{
		const_cast<std::vector<IManager*>&>(managers).push_back(manager_);
		return true;
	}

	return false;
}

bool ManagersContainer::CheckManager(const std::vector<IManager*>& managers, const IManager* manager)
{
	IManager* manager_ = const_cast<IManager*>(manager);
	const_cast<std::vector<IManager*>&>(managers).push_back(manager_);
	return true;
}

void ManagersContainer::ThreadFunc()
{
	while(!m_isExit)
	{
		std::vector<IManager*> managers;
		m_managers.CheckObjects(Ref(this, &ManagersContainer::RunManager, managers));
		for(std::vector<IManager*>::iterator it = managers.begin();
			it != managers.end(); it++)
		{
			(*it)->ManagerStop();
			delete (*it);
		}
		sleep(200);
	}
	
	std::vector<IManager*> managers;
	m_managers.CheckObjects(Ref(this, &ManagersContainer::CheckManager, managers));
	for(std::vector<IManager*>::iterator it = managers.begin();
		it != managers.end(); it++)
	{
		(*it)->ManagerStop();
		delete (*it);
	}
}

void ManagersContainer::OnStop()
{
}

void ManagersContainer::OnStart()
{
}

void ManagersContainer::Stop()
{
	m_isExit = true;
}

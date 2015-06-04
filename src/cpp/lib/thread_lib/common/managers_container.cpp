#include "managers_container.h"
#include "common\ref.h"

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

void ManagersContainer::RunManager(const IManager* manager)
{
	const_cast<IManager*>(manager)->ManagerFunc();
}

bool ManagersContainer::CheckManager(const IManager* manager)
{
	IManager* manager_ = const_cast<IManager*>(manager);
	manager_->ManagerStop();
	return true;
}

void ManagersContainer::ThreadFunc()
{
	while(!m_isExit)
	{
		std::vector<Thread*> threads;
		m_managers.ProcessingObjects(Ref(this, &ManagersContainer::RunManager));
		sleep(200);
	}

	m_managers.CheckObjects(Ref(this, &ManagersContainer::CheckManager));
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

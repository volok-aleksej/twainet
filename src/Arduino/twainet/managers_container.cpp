#include "managers_container.h"

/****************************************************************************************************************/
/*                                             ManagersContainer                                                */
/****************************************************************************************************************/
ManagersContainer::ManagersContainer()
: m_isExit(false)
{
	StartThread();
}

ManagersContainer::~ManagersContainer()
{
	StopThread();
}

void ManagersContainer::AddManager(IManager* manager)
{
	manager->ManagerStart();
	m_managers.insert(m_managers.end(), manager);
}

void ManagersContainer::RemoveManager(IManager* manager)
{
	manager->ManagerStop();
    for(twnstd::list<IManager*>::iterator it = m_managers.begin();
        it != m_managers.end(); ++it) {
        if(*it == manager) {
            m_managers.erase(it);
            break;
        }
    }
}

void ManagersContainer::ThreadFunc()
{
	while(!m_isExit)
	{
        for(twnstd::list<IManager*>::iterator it = m_managers.begin();
            it != m_managers.end(); ++it) {
            (*it)->ManagerFunc();
            if((*it)->IsStop()) {
                (*it)->ManagerStop();
                if((*it)->IsDestroyable()) {
                    delete (*it);
                }
            }
		}
	}
}

void ManagersContainer::OnStop()
{
    for(twnstd::list<IManager*>::iterator it = m_managers.begin();
        it != m_managers.end(); ++it) {
        (*it)->ManagerStop();
        if((*it)->IsDestroyable()) {
            delete (*it);
        }
    }
}

void ManagersContainer::OnStart()
{
}

void ManagersContainer::Stop()
{
	m_isExit = true;
}

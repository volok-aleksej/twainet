#include "thread_manager.h"

#include "common/ref.h"

ThreadManager::ThreadManager()
: m_isExit(false)
{
	Start();
}

ThreadManager::~ThreadManager()
{
	Stop();
	Join();
}

void ThreadManager::AddThread(Thread* thread)
{
	m_threads.AddObject(thread);
}

void ThreadManager::RemoveThread(Thread* thread)
{
	thread->StopThread();
}

bool ThreadManager::CheckThread(const std::vector<Thread*>& threads, const Thread* thread)
{
	Thread* thread_ = const_cast<Thread*>(thread);
	if(thread_->IsStopped())
	{
		const_cast<std::vector<Thread*>&>(threads).push_back(thread_);
		return true;
	}

	return false;
}

bool ThreadManager::StopThread(const std::vector<Thread*>& threads, const Thread* thread)
{
	Thread* thread_ = const_cast<Thread*>(thread);
	thread_->Join();
	const_cast<std::vector<Thread*>&>(threads).push_back(thread_);
	return true;
}

void ThreadManager::ThreadFunc()
{
	while(!m_isExit)
	{
		std::vector<Thread*> threads;
		m_threads.CheckObjects(Ref(this, &ThreadManager::CheckThread, threads));

		for(std::vector<Thread*>::iterator it = threads.begin(); it != threads.end(); it++)
		{
			delete *it;
		}

		sleep(200);
	}
}

void ThreadManager::OnStop()
{
	std::vector<Thread*> threads;
	m_threads.CheckObjects(Ref(this, &ThreadManager::StopThread, threads));
	
	for(std::vector<Thread*>::iterator it = threads.begin(); it != threads.end(); it++)
	{
		delete *it;
	}
}

void ThreadManager::OnStart()
{
}

void ThreadManager::Stop()
{
	m_isExit = true;
}

#include "thread_impl.h"
#include "thread_manager.h"

ThreadImpl::ThreadImpl()
{
}

ThreadImpl::~ThreadImpl()
{
}

void ThreadImpl::OnStop()
{
	ThreadManager::GetInstance().RemoveThread(this);
}

void ThreadImpl::OnStart()
{
	ThreadManager::GetInstance().AddThread(this);
}

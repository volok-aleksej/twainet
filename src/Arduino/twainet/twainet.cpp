#include "twainet.h"
#include "thread_manager.h"

extern "C" void twainetAppRun(void (*main)(void))
{
    while(true) {
        main();
        ThreadManager::GetInstance().SwitchThread();
    }
}
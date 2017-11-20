#ifndef DEFAULT_APP_H
#define DEFAULT_APP_H

#include <vector>
#include "appinterface.h"
#include "critical_section.h"
#include "module.h"

template<typename Application>
class DefaultApplication : public IApplication<Application>
{
public:
    DefaultApplication()
    {
    }
    
    virtual ~DefaultApplication()
    {
    }
    
    void AddModule(Module* module)
    {
        CSLocker locker(&m_cs);
        m_modules.push_back(module);
    }
protected:
    virtual void InitializeApplication()
    {
    }

    virtual void ShutdownApplication()
    {
        CSLocker locker(&m_cs);
        for(std::vector<IModule*>::iterator it = m_modules.begin();
            it != m_modules.end(); it++)
        {
            (*it)->Free();
        }
    }
    
protected:
    friend class IApplication<Application>;
    virtual void OnModuleCreationFailed(Twainet::Module module)
    {    
    }
    
    virtual void OnServerCreationFailed(Twainet::Module module)
    {
    }
    
    virtual void OnTunnelCreationFailed(Twainet::Module module, const char* sessionId)
    {
        CSLocker locker(&m_cs);
        for(std::vector<IModule*>::iterator it = m_modules.begin();
            it != m_modules.end(); it++)
        {
            if(module == (*it)->GetModule())
            {
                (*it)->OnTunnelCreationFailed(sessionId);
                break;
            }
        }
    }
    
    virtual void OnServerConnected(Twainet::Module module, const char* sessionId)
    {
        CSLocker locker(&m_cs);
        for(std::vector<IModule*>::iterator it = m_modules.begin();
            it != m_modules.end(); it++)
        {
            if(module == (*it)->GetModule())
            {
                (*it)->OnServerConnected(sessionId);
                break;
            }
        }
    }
    
    virtual void OnClientConnected(Twainet::Module module, const char* sessionId)
    {
        CSLocker locker(&m_cs);
        for(std::vector<IModule*>::iterator it = m_modules.begin();
            it != m_modules.end(); it++)
        {
            if(module == (*it)->GetModule())
            {
                (*it)->OnClientConnected(sessionId);
                break;
            }
        }
    }
    
    virtual void OnClientDisconnected(Twainet::Module module, const char* sessionId)
    {
        CSLocker locker(&m_cs);
        for(std::vector<IModule*>::iterator it = m_modules.begin();
            it != m_modules.end(); it++)
        {
            if(module == (*it)->GetModule())
            {
                (*it)->OnClientDisconnected(sessionId);
                break;
            }
        }
    }
    
    virtual void OnClientConnectionFailed(Twainet::Module module)
    {
        CSLocker locker(&m_cs);
        for(std::vector<IModule*>::iterator it = m_modules.begin();
            it != m_modules.end(); it++)
        {
            if(module == (*it)->GetModule())
            {
                (*it)->OnClientConnectionFailed();
                break;
            }
        }
    }
    
    virtual void OnClientAuthFailed(Twainet::Module module)
    {
        CSLocker locker(&m_cs);
        for(std::vector<IModule*>::iterator it = m_modules.begin();
            it != m_modules.end(); it++)
        {
            if(module == (*it)->GetModule())
            {
                (*it)->OnClientAuthFailed();
                break;
            }
        }
    }
    
    virtual void OnServerDisconnected(Twainet::Module module)
    {
        CSLocker locker(&m_cs);
        for(std::vector<IModule*>::iterator it = m_modules.begin();
            it != m_modules.end(); it++)
        {
            if(module == (*it)->GetModule())
            {
                (*it)->OnServerDisconnected();
                break;
            }
        }
    }
    
    virtual void OnModuleConnected(Twainet::Module module, const Twainet::ModuleName& moduleId)
    {
        CSLocker locker(&m_cs);
        for(std::vector<IModule*>::iterator it = m_modules.begin();
            it != m_modules.end(); it++)
        {
            if(module == (*it)->GetModule())
            {
                (*it)->OnModuleConnected(moduleId);
                break;
            }
        }
    }
    
    virtual void OnModuleDisconnected(Twainet::Module module, const Twainet::ModuleName& moduleId)
    {
        CSLocker locker(&m_cs);
        for(std::vector<IModule*>::iterator it = m_modules.begin();
            it != m_modules.end(); it++)
        {
            if(module == (*it)->GetModule())
            {
                (*it)->OnModuleDisconnected(moduleId);
                break;
            }
        }
    }
    
    virtual void OnModuleConnectionFailed(Twainet::Module module, const Twainet::ModuleName& moduleId)
    {
        CSLocker locker(&m_cs);
        for(std::vector<IModule*>::iterator it = m_modules.begin();
            it != m_modules.end(); it++)
        {
            if(module == (*it)->GetModule())
            {
                (*it)->OnModuleConnectionFailed(moduleId);
                break;
            }
        }
    }
    
    virtual void OnTunnelConnected(Twainet::Module module, const char* sessionId, Twainet::TypeConnection type)
    {
        CSLocker locker(&m_cs);
        for(std::vector<IModule*>::iterator it = m_modules.begin();
            it != m_modules.end(); it++)
        {
            if(module == (*it)->GetModule())
            {
                (*it)->OnTunnelConnected(sessionId, type);
                break;
            }
        }
    }
    
    virtual void OnTunnelDisconnected(Twainet::Module module, const char* sessionId)
    {
        CSLocker locker(&m_cs);
        for(std::vector<IModule*>::iterator it = m_modules.begin();
            it != m_modules.end(); it++)
        {
            if(module == (*it)->GetModule())
            {
                (*it)->OnTunnelDisconnected(sessionId);
                break;
            }
        }
    }
    
    virtual void OnMessageRecv(Twainet::Module module, const Twainet::Message& msg)
    {
        CSLocker locker(&m_cs);
        for(std::vector<IModule*>::iterator it = m_modules.begin();
            it != m_modules.end(); it++)
        {
            if(module == (*it)->GetModule())
            {
                (*it)->OnMessageRecv(msg);
                break;
            }
        }
    }
    
    virtual void OnInternalConnectionStatusChanged(Twainet::Module module, const char* moduleName, Twainet::InternalConnectionStatus status, int port)
    {
        CSLocker locker(&m_cs);
        for(std::vector<IModule*>::iterator it = m_modules.begin();
            it != m_modules.end(); it++)
        {
            if(module == (*it)->GetModule())
            {
                (*it)->OnInternalConnectionStatusChanged(moduleName, status, port);
                break;
            }
        }
    }
    
    virtual void OnModuleListChanged(Twainet::Module module)
    {
        CSLocker locker(&m_cs);
        for(std::vector<IModule*>::iterator it = m_modules.begin();
            it != m_modules.end(); it++)
        {
            if(module == (*it)->GetModule())
            {
                (*it)->OnModuleListChanged();
                break;
            }
        }
    }
    
private:
    CriticalSection m_cs;
    std::vector<IModule*> m_modules;
};

#endif/*DEFAULT_APP_H*/

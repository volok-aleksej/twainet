![twainet logotype](/logo.png)
# twainet - IPC and RPC library
## **Interprocess Communication(IPC) Functionality**
![ipc arhitecture](/doc/ipc_arch.png)
* Daemon — Coordinator monitors all running processes involved in the ipc, Local server allow you to communicate the processes belonging to different users.
* Direct communication between modules (processes) and single user connection through a secure tunnel between the modules(processes) of different users.
* Providing lists of modules (processes) available to connect list and modules (processes) that are already connected.
* Sending a message to another module(processes), both directly and through the co-ordinator or the server.
## **Remote Process Communication(RPC) Functionality**
![rpc arhitecture](/doc/rpc_arch.png)
* Server — Module that allows clients to communicate with each other.
* The ability to create 3 different types of tunnel: \
&nbsp;&nbsp;&nbsp;&nbsp;local — local tcp or udp connection between clients which are located on a single network \
&nbsp;&nbsp;&nbsp;&nbsp;external — udp connection between clients which are located behind a routers \
&nbsp;&nbsp;&nbsp;&nbsp;relay — tcp or udp connection which takes place through the server \
## **Components**
+ **IPC Components**
    - Deamon application
    - Twainet shared library
    - User applications
+ **RPC Component**
    - Server application
    - Client Aplication
## Security
+ Local connection(single connection between modules of single user) is not encrypted
+ Server connection and Tunnel connection is encrypted rsa-aes algoritm on both sides.
    - initiator are generated rsa public keys and are sent other side.
    - other sides are generated aes key and after them are encrypted, are sent initiator side.
    - Further communication encryption by aes keys.
 ## Sample
* main.cpp
```
#ifdef WIN32
#       include "stdafx.h"
#       include <windows.h>
#else
#       include <stdlib.h>
#       include <stdio.h>
#endif/*WIN32*/
#include <string.h>
#include <iostream>
#include "include/twainet.h"
#include "Application.h"
#include "common/common_func.h"
#include "thread_lib/thread/thread_manager.h"

#ifdef WIN32
int _tmain(int argc, _TCHAR* argv[])
#else
int main(int argc, char* argv[])
#endif/*WIN32*/
{
        printf("begin\n");
        Twainet::UseLog("/home/alexey/twainet.log");
        ApplicationTest::GetInstance().Run();
        return 0;
}
```
* test_module.h
```
#ifndef TEST_MODULE_H
#define TEST_MODULE_H

#include "include/module.h"

class TestModule : public Module
{
public:
        TestModule();
        virtual ~TestModule();

public:
        virtual void OnModuleListChanged();
        virtual void OnServerConnected(const char* sessionId);
private:
        template<class TMessage, class THandler> friend class DeamonMessage;
        void onMessage(const Test& test, Twainet::ModuleName path);
};

#endif/*TEST_MODULE_H*/
```
* test_module.cpp
```
#include "test_module.h"
#include "common/common_func.h"
#include <stdio.h>
#include <netinet/in.h>

TestModule::TestModule()
: Module("twntest", Twainet::IPV4, false)
{
    Twainet::UserPassword usr_pwd;
    strcpy(usr_pwd.m_user, "test");
    strcpy(usr_pwd.m_pass, "test");
    Twainet::SetUsersList(m_module, &usr_pwd, 1);
    Twainet::CreateServer(m_module, 5200, Twainet::IPV4);

    AddMessage(new DeamonMessage<Test, TestModule>(this));
}

TestModule::~TestModule()
{
}

void TestModule::OnModuleListChanged()
{
        Module::OnModuleListChanged();

        Twainet::ModuleName* names = 0;
        int sizeNames = 0;
        Twainet::GetExistingModules(GetModule(), names, sizeNames);
        names = new Twainet::ModuleName[sizeNames];
        sizeNames = Twainet::GetExistingModules(GetModule(), names, sizeNames);

        printf("moduleNames changed:\n");
        for(int i = 0; i < sizeNames; i++)
        {
                printf("%d:%s.%s.%s\n", i, names[i].m_name, names[i].m_host, names[i].m_connId);
        }
        printf("\n");

        delete names;
}

void TestModule::OnServerConnected(const char* sessionId)
{
        Module::OnServerConnected(sessionId);

        Twainet::ModuleName moduleName = {0};
        strcpy(moduleName.m_name, "twntest");
        strcpy(moduleName.m_host, sessionId);
        DeamonMessage<Test, TestModule> cnMsg(this);
        toMessage(cnMsg, moduleName);
}


void TestModule::onMessage(const Test& test, Twainet::ModuleName path)
{
  printf("test message\n");
}
```
* Application.h
```
#ifndef APPLICATION_H
#define APPLICATION_H

#include "include/twainet.h"
#include "include/default_app.h"
#include "test_module.h"

class ApplicationTest : public DefaultApplication<ApplicationTest>
{
public:
        ApplicationTest();
        ~ApplicationTest();
protected:
    virtual void InitializeApplication();    
    virtual void OnModuleCreationFailed(Twainet::Module module);
};

extern Twainet::TwainetCallback tc;

#endif/*APPLICATION_H*/
```
* Application.cpp
```
#ifdef WIN32
#       include "stdafx.h"
#else
#       include <string.h>
#       include <stdio.h>
#endif/*WIN32*/
#include "Application.h"
#include "common/common_func.h"
#include "common/common.h"
#include <string>
#include <time.h>

ApplicationTest::ApplicationTest()
{
}

ApplicationTest::~ApplicationTest()
{
}

void ApplicationTest::OnModuleCreationFailed(Twainet::Module module)
{
        Stop();
}

void ApplicationTest::InitializeApplication()
{
        AddModule(new TestModule);
}

```


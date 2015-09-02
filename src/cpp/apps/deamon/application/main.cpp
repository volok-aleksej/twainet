#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#	include <WinSock2.h>
#	include <windows.h>
#else
#	include <sys/types.h>
#	include <sys/stat.h>
#	include <stdlib.h>
#	include <stdio.h>
#endif/*WIN32*/

#include "application.h"
#include "utils/utils.h"
#include "common/file.h"
#include "common/process.h"
#include "common/service_manager.h"

void print_usage(const std::vector<std::string>& argvs)
{
    if (argvs.size() >=2)
    {

#ifdef WIN32
        printf("Usage: %s -h|start|stop|debug|install|uninstall\n", app_self_name().c_str());
#else
        printf("Usage: %s -h|start|stop|debug\n", argvs[0].c_str());
#endif
        printf("      -h          Show this help screen.\n");

#ifdef WIN32
        printf("      -start      Start as a service.\n");
        printf("      -stop       Stop service.\n");
        printf("      -debug      Don't run as a service.\n");
        printf("      -install    Install as service.\n");
        printf("      -uninstall  Remove service.\n");
#else
        printf("      -start      Start as a daemon.\n");
        printf("      -stop       Stop daemon.\n");
        printf("      -debug      Don't run as a daemon.\n");
#endif
        printf("\n");
    }
}

#ifdef WIN32
SERVICE_STATUS ServiceStatus;
SERVICE_STATUS_HANDLE hStatus;

void service_control_handler(DWORD request)
{
    switch(request)
    {
    case SERVICE_CONTROL_STOP:
    case SERVICE_CONTROL_SHUTDOWN:
        {
            // TODO stopping
			ServiceStatus.dwWin32ExitCode = ServerApplication::GetInstance().Stop();
            ServiceStatus.dwCurrentState = SERVICE_STOPPED;
            ::SetServiceStatus (hStatus, &ServiceStatus);
            return;
        }
    default:
        break;
    }
    // Report current status
    SetServiceStatus (hStatus, &ServiceStatus);
    return;
}


VOID WINAPI service_main(DWORD dwNumServicesArgs, LPSTR *lpServiceArgVectors)
{
    int error;
    ServiceStatus.dwServiceType = SERVICE_WIN32;
    ServiceStatus.dwCurrentState = SERVICE_START_PENDING;
    ServiceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN;
    ServiceStatus.dwWin32ExitCode = 0;
    ServiceStatus.dwServiceSpecificExitCode = 0;
    ServiceStatus.dwCheckPoint = 0;
    ServiceStatus.dwWaitHint = 0;
    hStatus = RegisterServiceCtrlHandlerA(ServerApplication::GetInstance().GetAppName().c_str(), (LPHANDLER_FUNCTION)service_control_handler);
    if (hStatus == (SERVICE_STATUS_HANDLE)0)
    {
        // Registering Control Handler failed
        return;
    }

    // We report the running status to SCM.
    ServiceStatus.dwCurrentState = SERVICE_RUNNING;
    SetServiceStatus (hStatus, &ServiceStatus);

    // run Service
    error = ServerApplication::GetInstance().Run();

    ServiceStatus.dwCurrentState = SERVICE_STOPPED;
    ServiceStatus.dwWin32ExitCode = error;
    SetServiceStatus(hStatus, &ServiceStatus);
    return;
}
#else
__sighandler_t old_sig_handler = NULL;

void signal_handler(int sig) 
{
	std::string pname = app_self_name();
	syslog(LOG_WARNING, "Service %s Received signal %d.", pname.c_str(), sig);
	switch(sig) 
	{
	case SIGHUP:
	case SIGPIPE:
		break;
	case SIGTERM:
	case SIGQUIT:
	case SIGKILL:
		std::string app_name = app_self_name();
		std::string pid_file = get_process_pid_filename();
		File f(pid_file);
		f.Delete();
		DeamonApplication::GetInstance().Stop();
		syslog(LOG_INFO, "Service %s Stopped.", pname.c_str());
		exit(0);
		break;
	}

	if(old_sig_handler && (__sighandler_t)(SIG_ERR) != old_sig_handler)
		old_sig_handler(sig);
}

#endif/*WIN32*/

int run_process()
{
#ifdef WIN32
	SERVICE_TABLE_ENTRYA ServiceTable[2];
	ServiceTable[0].lpServiceName = const_cast<LPSTR>(ServerApplication::GetAppName().c_str());
	ServiceTable[0].lpServiceProc = (LPSERVICE_MAIN_FUNCTIONA)service_main;
	ServiceTable[1].lpServiceName = NULL;
	ServiceTable[1].lpServiceProc = NULL;
	// Start the control dispatcher thread for our service
	StartServiceCtrlDispatcherA(ServiceTable);
#else
	old_sig_handler = signal(SIGHUP, signal_handler);
	signal(SIGTERM, signal_handler);
	signal(SIGINT, signal_handler);
	signal(SIGQUIT, signal_handler);
	signal(SIGPIPE, signal_handler);

	std::string app_name = app_self_name();
	pid_t pid = 0, sid;
	std::string pid_file = get_process_pid_filename();
	File f(pid_file);
	if(f.IsExist())
	{
		std::ifstream om(pid_file.c_str(), std::ios::in | std::ios::binary);
		om >> pid;
		if(pid != 0)
		{	
			// check process is running 
			syslog(LOG_INFO, "Daemonizing process %s already running with pid %d.", app_name.c_str(), pid);
			exit(EXIT_SUCCESS);
		}

		f.Delete();
	}

	syslog(LOG_INFO, "starting the daemonizing process %s", app_name.c_str());

	/* Fork off the parent process */
	pid = fork();
	if (pid < 0)
	{
		syslog(LOG_INFO, "fork failed on starting the daemonizing process %s", app_name.c_str());
		exit(EXIT_FAILURE);
	}

	if (pid > 0)
	{
		syslog(LOG_INFO, "fork process %d on starting the daemonizing process %s", pid, app_name.c_str());
		std::ofstream om(pid_file.c_str(), std::ios::out | std::ios::binary);
		om << pid;
		om.flush();
		exit(EXIT_SUCCESS);
	}

	/* Change the file mode mask */
	umask(0);

	/* Create a new SID for the child process */
	sid = setsid();
	if (sid < 0)
	{
		/* Log the failure */
		syslog(LOG_INFO, "fork failed on starting the daemonizing process %s. ", app_name.c_str());
		f.Delete();
		exit(EXIT_FAILURE);
	}

	/* Change the current working directory */
	if ((chdir("/")) < 0)
	{
		/* Log the failure */
		syslog(LOG_INFO, "change to / failed on starting the daemonizing process %s. ", app_name.c_str());
		f.Delete();
		exit(EXIT_FAILURE);
	}

	/* Close out the standard file descriptors */
	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);
	
	syslog(LOG_INFO, "entry child daemonizing process %s", app_name.c_str());
	DeamonApplication::GetInstance().Run();

	f.Delete();
	syslog(LOG_INFO, "exit child daemonizing process %s", app_name.c_str());
#endif/*WIN32*/

	return 0;
}

int  main(int argc, char* argv[])
{
	std::vector<std::string> args;
	for(int i = 0; i < argc; i++)
		args.push_back(argv[i]);

	std::string full_name;
	std::string param;
	if (args.size() > 0)
	{
		full_name = args[0];
	}

	if (args.size() > 1)
	{
		param = args[1];
	}
	
	if (param == "-h" || param == "h")
	{
		print_usage(args);
	}
	else if (param == "-install" || param == "install")
	{
		full_name.insert(0, "\"");
		full_name.append("\"");
		ServiceManager manager(DeamonApplication::GetAppName());
		manager.Install(DeamonApplication::GetDescription(), full_name);
	}
	else if (param == "-uninstall" || param == "uninstall")
	{
		ServiceManager manager(DeamonApplication::GetAppName());
		manager.Remove();
	}
	else if (param == "-start" || param == "start")
	{
#ifdef WIN32
		ServiceManager manager(ServerApplication::GetAppName());
		manager.Start();
#else
		return run_process();
#endif/*WIN32*/
	}
	else if (param == "-stop" || param == "stop")
	{
		ServiceManager manager(DeamonApplication::GetAppName());
		manager.Stop();
	}
	else if(param == "-debug" || param == "debug")
	{
		return DeamonApplication::GetInstance().Run();
	}
	else if (param.empty())
	{
		return run_process();
	}
    
	return 0;
}

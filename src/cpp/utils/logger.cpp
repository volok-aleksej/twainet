#include "logger.h"
#include <stdarg.h>
#include <string.h>
#include <time.h>
#include "utils/utils.h"

Logger::Logger()
	: m_logFile(0)
{
}

Logger::~Logger()
{
	if(m_logFile)
		delete m_logFile;
}

void Logger::Log(TypeLog type, const char* prototype, ...)
{
	CSLocker locker(&m_cs);
	char printdata[1024] = {0};
	va_list argptr;
	va_start(argptr, prototype);
	vsnprintf(printdata, 1024, prototype, argptr);
	va_end(argptr);
	std::string strtype = GetStringType(type);
	time_t t;
	time(&t);
	std::string time_ = CommonUtils::FormatTime(t);
    
//    printf("%s %s %s", time_.c_str(), strtype.c_str(), printdata);
    
    if(!m_logFile)
    {
        return;
    }
	m_logFile->Write(time_.c_str(), time_.size());
	m_logFile->Write(strtype.c_str(), strtype.size());
	m_logFile->Write(printdata, strlen(printdata));
}

void Logger::SetLogFile(const std::string& logFileName)
{
	CSLocker locker(&m_cs);
	if(m_logFile)
		delete m_logFile;

	m_logFile = new File(logFileName);
	m_logFile->Delete();
}

std::string Logger::GetStringType(TypeLog type)
{
	switch(type)
	{
	case LOG_WARNING:
		return " LOG_WARNING:\t";
	case LOG_ERROR:
		return " LOG_ERROR:\t\t";
	case LOG_INFO:
		return " LOG_INFO:\t\t";
	}

	return "";
}

#include "CLog.h"
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

CLog* CLog::m_log = NULL;

CLog* CLog::getInstance()
{
	if(m_log == NULL)
	{
		m_log = new CLog();
	}
	return m_log;
}

CLog::CLog()
{
	ctime.updateTime();
	ctime.setMday(ctime.getMday() + 1);
	ctime.setHour(0);
	ctime.setMin(0);
	ctime.setSec(0);
	starttime = ctime.timeSec();
}

CLog::~CLog()
{
	if(logHandle > 0)
		close(logHandle);
}

bool CLog::Init(const char* path)
{
	if(mkdir(path, 0755) == -1)
	{
		if(errno != EEXIST)
		{
			fprintf(stderr,"create log path failed\n");
			return false;
		}		
	}
	strcpy(m_logpath, path);
	ctime.updateTime();
	std::string timestr = ctime.timeToDay();
	sprintf(m_logfile, "%s/%s.log", m_logpath, timestr.c_str());
	logHandle = open(m_logfile, O_CREAT|O_RDWR|O_APPEND, 0644);
	if(logHandle < 0)
	{
		fprintf(stderr,"create log file failed\n");
		return false;
	}
	return true;
}

void CLog::Log(int level, const char * loginfo)
{
	const char *infos[4] = {"ERROR", "INFO", "WARN", "DEBUG"};
	m_mutex.Lock();
	checkLogTime();
	char logbuf[256];
	if(logHandle > 0)
	{
		ctime.updateTime();
		sprintf(logbuf, "[%s]%s: %s\n", ctime.timeToStr().c_str(), infos[level], loginfo);
		int handle = write(this->logHandle, logbuf, strlen(logbuf));
        if(handle < 0)
		{
			fprintf(stderr, "write log file err\n");
		}
	}
	m_mutex.UnLock();
}

void CLog::Error(const char *pFmt, ...)
{
	va_list valist;
	char Buf[BUFSIZE]={'\0'};
	va_start(valist, pFmt);
	vsnprintf(Buf, sizeof(Buf)-1, pFmt, valist);        
	va_end(valist);       
	Log(LOG_ERROR, Buf);
}

void CLog::Info(const char  *pFmt, ...)
{
	va_list valist;
	char Buf[BUFSIZE]={'\0'};
	va_start(valist, pFmt);
	vsnprintf(Buf, sizeof(Buf)-1, pFmt, valist);        
	va_end(valist);       
	Log(LOG_INFO, Buf);	
}

void CLog::Debug(const char *pFmt, ...)
{
	va_list valist;
	char Buf[BUFSIZE]={'\0'};
	va_start(valist, pFmt);
	vsnprintf(Buf, sizeof(Buf)-1, pFmt, valist);        
	va_end(valist);       
	Log(LOG_DEBUG, Buf);	
}

void CLog::Warn(const char  *pFmt, ...)
{
	va_list valist;
	char Buf[BUFSIZE]={'\0'};
	va_start(valist, pFmt);
	vsnprintf(Buf, sizeof(Buf)-1, pFmt, valist);        
	va_end(valist);      
	Log(LOG_WARN, Buf);	
}


bool CLog::checkLogTime()
{
	if(difftime(time(NULL), starttime) >= 0)
	{
		memset(this->m_logfile, 0, sizeof(this->m_logfile));
		ctime.updateTime();
		std::string timestr = ctime.timeToDay();
		sprintf(m_logfile, "%s/%s.log", m_logpath, timestr.c_str());
		int handle = open(m_logfile, O_CREAT | O_RDWR | O_APPEND, 644);
        if(handle > 0)
		{
			close(logHandle);
			this->logHandle = handle;
		}
		ctime.setMday(ctime.getMday()+1);
		ctime.setHour(0);
		ctime.setMin(0);
		ctime.setSec(0);
		this->starttime = ctime.timeSec();
		return true;
	}

	return false;
}

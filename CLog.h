#ifndef __LOG_H
#define __LOG_H

#include "CTime.h"
#include <stdarg.h>
#include "Mutex.h"

#define BUFSIZE 2048

class CLog
{
public:
	CLog();
	virtual ~CLog();
	
	static CLog* getInstance();
	bool Init(const char* path);

	void Error(const char *pFmt, ...);
    void Info(const char  *pFmt, ...);
    void Debug(const char *pFmt, ...);
    void Warn(const char  *pFmt, ...);     

    void Log(int level, const char * loginfo);
private:
	bool checkLogTime();
	static CLog *m_log;
	CLog(const CLog& clog);
	CLog& operator=(const CLog& clog);

	Mutex m_mutex;
	int m_logLevel;
	char m_logpath[100];
	char m_logfile[200];
	int logHandle;
	enum { LOG_ERROR = 0, LOG_INFO, LOG_WARN, LOG_DEBUG};
	CTime ctime;
	time_t starttime;
};

#define LogPrint CLog::getInstance()

#endif //__LOG_H

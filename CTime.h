#ifndef __TIME_H
#define __TIME_H

#include <sys/time.h>
#include <signal.h>
#include <stddef.h>
#include <iomanip>
#include <string>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

//普通时间类，以后根据情况扩展

class CTime
{
public:
    CTime();
	CTime(const CTime& ctime);
	CTime(const time_t& tim);
	CTime(const struct tm &tmvar);
    virtual ~CTime(){}

	CTime operator=(const CTime &ctime)
	{
		if(this != &ctime)
		{
			this->tm_ = ctime.tm_;
			this->time_ = ctime.time_;
		}
		return *this;
	}
	CTime operator-(const CTime &ctime)
	{
		return CTime(this->time_ - ctime.time_);
	}
	CTime operator+(const CTime& ctime)
	{
		return CTime(this->time_ + ctime.time_);
	}
	int getYear() const { return tm_.tm_year;}
	int getMon()  const { return tm_.tm_mon; }
	int getMday() const { return tm_.tm_mday; }
	int getWday() const { return tm_.tm_wday;}
	int getYday() const { return tm_.tm_yday;}
	int getHour() const { return tm_.tm_hour;}
	int getMin()  const { return tm_.tm_min; }
	int getSec()  const { return tm_.tm_sec; }

	void setYear(int year);
	void setMon(int mon);
	void setMday(int mday);
	void setHour(int hour);
	void setMin(int min);
	void setSec(int sec);
    void updateTime();
	void updateTime(time_t tim);
	std::string timeToStr();
	std::string timeToDay();
	time_t timeSec() const
	{
		return this->time_;
	}
private:
	struct tm tm_;
	time_t time_;
};


//专门设置定时任务类
class CTimeTask
{
public:
	typedef void(*func)(int signo, siginfo_t* info, void* context);
	
    CTimeTask();
    virtual ~CTimeTask();

    int excuteTask(int msec, func onFunc, int mode = 1);

	static CTimeTask* getInstance();
	timer_t getTimeID() { return timeID; }
private:
    timer_t timeID;  
    struct sigevent evp;
	static CTimeTask *timeTask;
	CTimeTask(const CTimeTask&);
	CTimeTask& operator=(const CTimeTask&);
};

#define TimerTask CTimeTask::getInstance()

//日期类处理年月日
class CDate
{
public:
	
	static std::string DateSpan(std::string strDate, long nSpan, char cDelimiter = '-');
	static void   GetCurrentMonth(std::string &strFrom, std::string &strTo);
	static int    GetWeekDay(std::string strDate);
	static int    GetWeek(std::string strDate);
	static long   MinuteDiff(std::string strDateFrom, std::string strDateTo);
	static long   DateDiff(std::string strDateFrom, std::string strDateTo);
	static int    YearDiff(std::string strDateFrom, std::string strDateTo);
	static std::string GetMonthStart(std::string strDate);
	static std::string GetMonthEnd(std::string strDate);
	static std::string GetYear(std::string strDate);
	static std::string GetYearMonth(std::string strDate);
	static long   days_from_8zeros(char *date8);  
	static std::string FilterNumber(const std::string &str);
	static std::string GetWeekStartEnd(int year,int weekorder);
	static int    GetMonthDays(int year,int month);
	static int    WhatTime();
private:
	CDate(const CDate&);
	CDate& operator=(const CDate&);
};



#endif //__TIME_H

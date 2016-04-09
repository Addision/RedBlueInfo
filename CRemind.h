#ifndef __REMIND_H
#define __REMIND_H

#include <string>
#include <signal.h>
#include <stddef.h>
#include <iomanip>
#include <time.h>

class CRemind
{
public:
    CRemind(std::string path);
    virtual ~CRemind();

	static bool readConfig();
	void Start();
	static std::string WebAddr;
	static int Port;
	static int Interval;
	static std::string WebSpider;
	static bool SingleMulti; //true 单注 false 多注
	static int Bets;
	static std::string RecvEmail;
	static std::string SendEmail;
    static std::string config;
	static bool simple;
    static void timeProc(int signo, siginfo_t* info, void* context);

private:
	std::string workPath;
	static time_t preTime;
};

#endif








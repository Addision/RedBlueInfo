#include "CLog.h"
#include "CRemind.h"

#include <exception>
#include <typeinfo>
#include <vector>
#include <iostream>
#include <stdio.h>
#include <signal.h>
#include <sys/resource.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

void signal_hup(const int signum);
void signal_term(const int signum);
void signal_pipe(const int signum);
bool checkExists(const char* shellstr, const char* namestr);
void usage();

/****************************************
 SIGHUP 处理函数，重新启动服务

 ***************************************/
void signal_hup(const int signum)
{
	printf("signal handled: %s\n", strsignal(signum));
	exit(-1);
}

/****************************************
 SIGTERM/SIGINT 处理函数，停止服务
 ***************************************/
void signal_term(const int signum)
{
	printf("signal handled: %s\n", strsignal(signum));
	exit(-1);
}

/****************************************
 SIGPIPE 处理函数，忽略它
 ***************************************/
void signal_pipe(const int signum)
{
	printf("signal handled: %s\n", strsignal(signum));
	exit(-1);
}

/****************************************
检查是否已经存在
shellstr = "ps aux | grep -v grep | grep Remind"
namestr = "Remind"

 ***************************************/

bool checkExists(const char* shellstr, const char* namestr)
{
	char buf[100];
    int cnt = 0;
	memset(buf, 0, sizeof(buf));
	FILE *fp = popen(shellstr, "r");
	if(fp == NULL)
	{
		fprintf(stderr, "popen err\n");
		return false;
	}
	while(fgets(buf, sizeof(buf), fp) != NULL)
	{
		if(strstr(buf, namestr)!=NULL)
		{
			++cnt;
		}
	}
	pclose(fp);
	if(cnt >= 2)
		return false;
    return true;
}
/****************************************
the usage
*****************************************/
void usage()
{
	printf("此程序是进行实时抓取双色球信息，并进行核对是否中奖，如果中奖会以邮件形式发送到用户邮箱，如果没有则会发送当日开奖信\n");
}

/***********************main******************************/
int main(int argc, char *argv[])
{
	const char* version = "v0.1";
	std::string path = ".";
    //Check whether the program has started
	bool started = checkExists("ps aux | grep -v grep |grep -v gdb | grep Remind", "Remind");
	if(!started)
	{
        fprintf(stderr,"the program has started\n");
		exit(-1);
	}

	//set the log
	LogPrint->Init("./log");
	LogPrint->Info("the sever start....");
	
	bool do_daemon = false;
	int opt = 0;
	while((opt = getopt(argc, argv, "hvds")) != -1)
	{
		switch(opt)
		{
		case 'h':
			usage();
			exit(0);
		case 'v':
			printf("the version : %s\n", version);
			exit(0);
		case 'd':
			do_daemon = true;
			break;
		case 's':
			CRemind::simple = true;
			break;
		default:
			usage();
			//exit(0);
		}
	}
	//set the deamon
	if(do_daemon)
	{
		if(daemon(1,0) == -1)
		{
			fprintf(stderr, "failed to set deamon\n");
			exit(-1);
		}
	}

	if(do_daemon)
	{
		if(sigignore(SIGHUP) == -1)
		{
			fprintf(stderr, "failed to ignore SIGHUP\n");
			exit(-1);
		}
	}

	//set the resource
	struct rlimit rlim;
	struct rlimit rlim_new;
	if(getrlimit(RLIMIT_CORE, &rlim) == 0)
	{
		rlim_new.rlim_cur = rlim_new.rlim_max = RLIM_INFINITY;
		if(setrlimit(RLIMIT_CORE, &rlim_new) !=0)
		{
			rlim_new.rlim_cur = rlim_new.rlim_max = rlim.rlim_max;
			setrlimit(RLIMIT_CORE, &rlim_new);
		}
	}

	if((getrlimit(RLIMIT_CORE, &rlim) != 0) || rlim.rlim_max == 0)
	{
		LogPrint->Error("set resource limit failed");
		exit(-1);
	}

	try{
		if(SIG_ERR == signal(SIGHUP, signal_hup))
		{
			LogPrint->Error("can't install sigup signal");
		}
		if(SIG_ERR == signal(SIGTERM, signal_term))
		{
			LogPrint->Error("can't install sigterm signal");
		}
		if(SIG_ERR == signal(SIGINT, signal_term))
		{
			LogPrint->Error("can't install sigint signal");
		}
		if(SIG_ERR == signal(SIGPIPE, signal_pipe))
		{
			LogPrint->Error("can't install sigpipe signal");
		}

	}catch(std::exception &e)
	{
		LogPrint->Error("exception catch: %s", e.what());
	}

	CRemind Remind("./");
	Remind.Start();
		
	while(true)
	{
		sleep(120);
	}
    LogPrint->Info("the server stop...");

    return 0;
}



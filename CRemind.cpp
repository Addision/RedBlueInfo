#include "CRemind.h"
#include "CConfig.h"
#include "CLog.h"
#include "CTime.h"
#include "CSpider.h"
#include "CLottery.h"
#include "CSendEmail.h"
#include <iostream>
#include <string>

std::string CRemind::WebAddr = "";
int CRemind::Port = 0;
int CRemind::Interval = 0;
std::string CRemind::WebSpider = "";
bool CRemind::SingleMulti = true; //true 单注 false 多注
std::string CRemind::RecvEmail = "";
std::string CRemind::SendEmail = "";
std::string CRemind::config = "";
int CRemind::Bets = 0;
bool CRemind::simple = false;
time_t CRemind::preTime = 0;

CRemind::CRemind(std::string path):workPath(path)
{
	config = workPath+"/config";
	CTime ctime;
	ctime.setHour(10);
	ctime.setMin(0);
	ctime.setSec(0);
	CRemind::preTime = ctime.timeSec();
}

CRemind::~CRemind()
{}

/****************************************
readconfig 读取配置文件内容
****************************************/
bool CRemind::readConfig()
{
	if(!Config->Load(config))
		return false;
	else
	{
		WebAddr = Config->getValueString("Mail", "webaddr", "smtp.126.com");
		Port = Config->getValueInt("Mail", "port", 25);
		Interval = Config->getValueInt("contants", "interval", 10);
		WebSpider = Config->getValueString("contants", "webspider", "http://www.zhcw.com/");
		SingleMulti = Config->getValueBool("contants", "SingleMulti", true);
        RecvEmail = Config->getValueString("Mail", "RecvEmail", "");
		SendEmail = Config->getValueString("Mail", "SendEmail", "");
        Bets = Config->getValueInt("contants", "bets", 0);
		return true;
	}
}

void CRemind::Start()
{
	//read config
	if(!readConfig())
	{
		LogPrint->Error("read config failed");
		return;
	}
	//excute task
	TimerTask->excuteTask(1*30*1000, timeProc);
}

void CRemind::timeProc(int signo, siginfo_t* info, void* context)
{
	//check the date
	time_t ti = time(NULL);
	CTime ctime;
	int day = ctime.getWday();
	if(0 == day || 2 == day || 4 == day || 6 == day)
		return;

	if(difftime(ti,  CRemind::preTime) < 0)
		return;
	else{
		ctime.setMday(ctime.getMday() + 1);
		ctime.setHour(10);
		ctime.setMin(0);
		ctime.setSec(0);
		CRemind::preTime = ctime.timeSec();
	}	
	//Crawling lottery information
	CSpider spider;
	std::string page = spider.downLoad();
	spider.parseHtml(page);
	std::string redBlue = spider.getRedBlue();
	//std::cout << redBlue << "\n";
	if(redBlue.empty())
	{
		LogPrint->Info("crawling lottery information Err");
		return;
	}
	LogPrint->Info("crawling lottery information OK");
	if(spider.writeFile())
	{
		LogPrint->Info("write the record to file OK");
	}	
	LogPrint->Info("双色球开奖号码：%s", redBlue.c_str());
	//check the data
    if(CLottery::getInstance()->checkWin(redBlue))
	{
		LogPrint->Info("check the data OK");
	}
	//send email note
	std::string strLotInfo = "双色球第";
	strLotInfo += spider.getCycles();
	strLotInfo += "期\n开奖号码：" + redBlue;	
	strLotInfo += "\n";
	if(!CRemind::simple)
	{
		std::string otherinfo = spider.getOtherInfos();
		if(!otherinfo.empty())
		{
			strLotInfo += otherinfo;
			strLotInfo +="\n";
		}
		std::vector<winRes> winVec = spider.getWinVec();
		for(int i=0;i<winVec.size(); ++i)
		{
			struct WinRes *winRes = winVec[i];
			strLotInfo += winRes->prizelevel + "              ";
			strLotInfo += winRes->prizeNum + "人             ";
			strLotInfo += winRes->prize;
			strLotInfo += "\n";
		}
	}
	if(CLottery::getInstance()->getLotRes())
	{
		strLotInfo += "恭喜你！！！ \n";
		strLotInfo += CLottery::getInstance()->winLotStr();
		CLottery::getInstance()->multiVecClear();
	}
	else
	{
		strLotInfo += "\n很抱歉你未中奖...";
	}
	//send email
	LogPrint->Info("start send email");
	CSendEmail::getInstance()->SendEmail(strLotInfo);
	LogPrint->Info("end send ");
}



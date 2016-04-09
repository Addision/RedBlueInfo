#ifndef __SPIDER_H
#define __SPIDER_H

#include <string>
#include <curl/curl.h>
#include <curl/easy.h>
#include <vector>
#include <iconv.h>

typedef struct WinRes
{
	std::string prizelevel;
	std::string prizeNum;
	std::string prize;
	WinRes()
	{}
}*winRes;

class CSpider
{
public:
    CSpider();
    virtual ~CSpider();
	std::string downLoad();
	std::string getRedBlue() { return redblue;}
	std::string getOtherInfos() { return otherinfo;}
	std::vector<winRes> getWinVec() { return winVec; }
	void parseHtml(std::string& strHtml);
	std::string getCycles();
	bool writeFile();
private:
	std::string Cycles();
	std::string trim(std::string& str)
	{
		str.erase(0, str.find_first_not_of(" "));
		str.erase(str.find_last_not_of(" ")+1);
		return str;
	}
private:
	CURL *curl;
	std::string redblue;
	std::string otherinfo;
	std::vector<winRes> winVec;
	std::string cyclestr;
};


#endif //spider










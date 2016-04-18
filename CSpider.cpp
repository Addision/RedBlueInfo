#include "CSpider.h"
#include <time.h>
#include <string.h>
#include <stdio.h>
#include "CRemind.h"
#include "CLog.h"

#include <ParserDom.h>
#include <utils.h>
#include <iostream>
#include <fstream>

using namespace htmlcxx;
//gbk utf convert
int code_convert(char *from_charset,char *to_charset,char *inbuf,unsigned int inlen,char *outbuf,unsigned int outlen)  
{  
	iconv_t cd;  
	int rc;  
	char **pin = &inbuf;  
	char **pout = &outbuf;  
  
	cd = iconv_open(to_charset,from_charset);  
	if (cd==0)  
		return -1;  
	memset(outbuf,0,outlen);  
	if (iconv(cd,pin,&inlen,pout,&outlen) == -1)  
		return -1;  
	iconv_close(cd);  
	return 0;  
}  
  
int u2g(char *inbuf,int inlen,char *outbuf,int outlen)  
{  
	return code_convert("utf-8","gb2312",inbuf,inlen,outbuf,outlen);  
}  
  
int g2u(char *inbuf,size_t inlen,char *outbuf,size_t outlen)  
{  
	return code_convert("gb2312","utf-8",inbuf,inlen,outbuf,outlen);  
}  

size_t write_data(char* ptr, size_t size, size_t nmemb, std::string* stream)
{
	if(stream == NULL)
		return 0;
	unsigned int sizes = size*nmemb;
	stream->append(ptr, sizes);
	return sizes;
}

CSpider::CSpider()
{
	cyclestr = Cycles();
	curl_global_init(CURL_GLOBAL_ALL);  
    curl=curl_easy_init();	
}

CSpider::~CSpider()
{
    curl_easy_cleanup(curl);
}

std::string CSpider::Cycles()
{
	int cycles = 0;
	char cyclesbuf[10];
	struct tm *ptr;
	time_t ti = time(NULL);
	ptr = localtime(&ti);
	char timebuf[8];
	strftime(timebuf, sizeof(timebuf), "%W", ptr);
	cycles = atoi(timebuf);
	switch(ptr->tm_wday)  
	{
	case 2:
	case 3:
		cycles = cycles * 3 - 4;
		break;
	case 4:
	case 5:
	case 6:
		cycles = cycles * 3 - 3;
		break;
	case 0:
		cycles = cycles * 3 - 2;
	case 1:
		cycles = (cycles-1) * 3 - 2; //上一周开奖结果
		break;	   
	default:
		LogPrint->Error("get cycles error");
		break;
	}
	sprintf(cyclesbuf, "%d%03d", ptr->tm_year+1900, cycles);
	
    return std::string(cyclesbuf);
}

std::string CSpider::getCycles()
{
	if(!cyclestr.empty())
	{
		return cyclestr;
	}
}

std::string CSpider::downLoad()
{
	std::string pagebuf;
	
	char url[200];
	memset(url, '\0', sizeof(url));
	strcpy(url, "");
	strcat(url, CRemind::WebSpider.c_str());
	strcat(url, getCycles().c_str());

	CURLcode res;
	LogPrint->Info("download page url:%s", url);
	curl_easy_setopt(curl, CURLOPT_URL, url);
	curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 60);
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, 60);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &pagebuf);
    if((res = curl_easy_perform(curl)) == CURLE_OK)
	{
		LogPrint->Info("download page ok");
	}
    else
	{
		const char * err = curl_easy_strerror(res);
		LogPrint->Error(err);
	}
	return pagebuf;
}

void CSpider::parseHtml(std::string& strHtml)
{
	char pagebuf[100000];
	memset(pagebuf, '\0', sizeof(pagebuf));
	g2u(const_cast<char*>(strHtml.c_str()), strHtml.size(), pagebuf, 100000);
	strHtml = pagebuf;
	trim(strHtml);
	
	strHtml = HTML::strip_comments(strHtml); //跳过注视
	HTML::ParserDom parser;
	tree<HTML::Node> dom = parser.parseTree(strHtml);
	tree<HTML::Node>::iterator it = dom.begin();
	tree<HTML::Node>::iterator end = dom.end();

	WinRes *win = NULL;
	int k = 0;
	//parse the red and blue number
	for(;it != end; ++it)
	{
		if(!it->isTag())
		{
			continue;	
		}

		if(it->text().compare("<font color=red>") == 0)
		{
			it->parseAttributes();
			std::string str = it.node->first_child->data;
			redblue += str;
			redblue += " ";
		}
		if(it->text().compare("<font color=blue>") == 0)
		{
			it->parseAttributes();
			std::string str = it.node->first_child->data;
			redblue += str;
			//std::cout << redblue << "\n";
		}

		if(it->text().compare("<TD align=middle height=18 width=\"33%\">") == 0)
		{
			it->parseAttributes();
			std::string str = it.node->first_child->data;
			win = new WinRes;
			win->prizelevel = str;
		}
        if(it->text().compare("<TD height=18 width=\"33%\" align=center>") == 0)
		{
			it->parseAttributes();
			std::string str = it.node->first_child->data;
			if(k == 0)
			{
				win->prizeNum = str;
				++k;
			}
			else if(k == 1){
				k = 0;
				win->prize = str;
                winVec.push_back(win);
			}
		}

		if(strcmp(it->tagName().c_str(), "td") == 0)
		{
			it->parseAttributes();
			std::string str = it.node->first_child->data;
			int pos = str.find("投注总额为");
			if(pos != std::string::npos)
			{
				otherinfo = str;
				otherinfo += "\n";
			}
		}

		if(strcmp(it->tagName().c_str(), "br") == 0 || it->text().find("<br>") !=std::string::npos)
		{
			tree<HTML::Node>::iterator it2 = dom.begin(it);
			it2->parseAttributes();
			std::string str = it2->text();
			trim(str);
			if(str.size() > 0)
			{
				otherinfo += it2->text();
				otherinfo += "\n";
			}
		}
	}
}

bool CSpider::writeFile()
{
	std::ofstream out;
	out.open("record",std::ios::out | std::ios::app);
	if(out.good())
	{
		out<<this->getCycles()<< " "<<this->getRedBlue() << "\n";
	}
	out.flush();
	out.close();
}



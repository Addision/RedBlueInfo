#include "CLottery.h"
#include "CConfig.h"
#include <algorithm>
#include <iterator>   
#include <iostream>
#include "CRemind.h"
#include <stdio.h>
#include "CLog.h"

CLottery* CLottery::cLot = NULL;
CLottery* CLottery::getInstance()
{
	if(cLot == NULL)
		cLot = new CLottery();
	return cLot;
}
CLottery::CLottery()
{
	winStr = "";
}

CLottery::~CLottery()
{}

void CLottery::multiVecClear()
{
	if(multiVec.size() > 0)
	{
		std::vector<Record*>::iterator it = multiVec.begin();
		for(;it != multiVec.end(); ++it)
		{
			delete (struct Record*)(*it);
		}
	}
	multiVec.clear();
}

bool CLottery::checkWin(const std::string& strRedBlue)
{
	bool res = false;
	int pos = 0;

	if(CRemind::SingleMulti) //单注情况
	{
		std::string strLot = "";
		loadLot(strLot);
		dealwithCheck(strLot, strRedBlue);
	}
	else //多注情况
	{
		std::vector<std::string> vecLot;
		loadLot(vecLot);
		for(int i=0; i<vecLot.size(); ++i)
		{
			dealwithCheck(vecLot[i], strRedBlue);
		}		
	}

	for(int i=0;i<multiVec.size(); ++i)
	{
		Record *record = (struct Record*)multiVec[i];
		//std::cout << record->lot << " == " << record->lotStr <<"\n";
	}
	if(multiVec.size() > 0)
	{
		res = true;
	}
	return res;
}

void CLottery::loadLot(std::string& lotStr)
{
	lotStr = Config->getValueString("single-lottery", "keyLot", "");
}
void CLottery::loadLot(std::vector<std::string>& vec)
{
	Config->getStringList("multi-lottery", "keyLot", vec);
}

std::string CLottery::winLotStr()
{	
	for(int i = 0; i< multiVec.size(); ++i)
	{
		Record *record = (struct Record*)multiVec[i];
		switch(record->lot)
		{
		case 1:
			winStr += "一等奖 中奖号码: "; break;
		case 2:
			winStr += "二等奖 中奖号码: "; break;
		case 3:
			winStr += "三等奖 中奖号码: "; break;
		case 4:
			winStr += "四等奖 中奖号码: "; break;
		case 5:
			winStr += "五等奖 中奖号码: "; break;
		case 6:
			winStr += "六等奖 中奖号码: "; break;
		}
		winStr += record->lotStr;
		winStr +="\n";
	}

	return winStr;
	
}

void CLottery::dealwithCheck(const std::string& strLot, const std::string& strRedBlue)
{
	int iblue, ired, lot;
	iblue = ired = lot = 0;
    int pos = 0;
	Record *record = NULL;
	std::string strLotTmp = strLot;
	pos = strRedBlue.find_last_of(' ');
	std::string blue = strRedBlue.substr(pos+1);
	std::string red = strRedBlue.substr(0, pos);
	for(int i = 0;i < 6; ++i)
	{
		pos = strLotTmp.find_first_of(' ');
		strLotTmp = strLotTmp.substr(pos + 1);
	}
	std::string singleBlue = strLotTmp;
	std::string singleRed = strLot.substr(0, strLot.find(strLotTmp));

	if(singleBlue.find(blue) != std::string::npos)
	{
		++iblue;
	}
	std::string singleRedtmp = singleRed;
	std::string tmp;
	pos = singleRedtmp.find_first_of(' ');
	while(pos != std::string::npos)
	{
		tmp = singleRedtmp.substr(0, pos);
		if(red.find(tmp) != std::string::npos && !tmp.empty())
		{
			++ired;
			//std::cout << "red == "<<tmp <<std::endl;	
		}
		singleRedtmp = singleRedtmp.substr(pos + 1);
		pos = singleRedtmp.find_first_of(' ');
	}
	if(iblue > 0)
	{
		switch(ired)
		{
		case 0:
		case 1:
		case 2:
			lot = 6; break;				
		case 3:
			lot = 5; break;
		case 4:
			lot = 4; break;
		case 5:
			lot = 3; break;
		case 6:
			lot = 1; break;
		default:
			lot = 0; break;
		}
	}
	else
	{
		switch(ired)
		{
		case 4:
			lot = 5; break;
		case 5:
			lot = 4; break;
		case 6:
			lot = 2; break;
		default:
			lot = 0; break;
		}
	}
	if(lot > 0)
	{
		record = new Record;
		record->lot = lot;
		record->lotStr = strLot;
		multiVec.push_back(record);
	}
}



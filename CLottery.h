#ifndef __LOTTERY_H
#define __LOTTERY_H

#include <vector>
#include <map>
#include <string>

const int Max = 20;

struct Record
{
	int lot;
	std::string lotStr;
	Record():lot(0), lotStr("")
	{}
};
	
class CLottery
{
public:
    CLottery();
    virtual ~CLottery();  
	static CLottery* getInstance();
	bool checkWin(const std::string& strRedBlue);
	std::string winLotStr();
	bool getLotRes() { return multiVec.size() >0 ? true:false;}
	void multiVecClear();
private:
	void loadLot(std::string& lotStr);
	void loadLot(std::vector<std::string>& vec);
	void dealwithCheck(const std::string& strLot,const std::string& strRedBlue);
private:

	std::vector<Record*> multiVec; //记录多注中奖情况
	std::string winStr;
	static CLottery* cLot;
};

#define CLot CLottery::getInstance()

#endif //__LOTTERY_H

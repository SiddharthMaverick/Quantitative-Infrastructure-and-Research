#ifndef __simulatorManager__
#define __simulatorManager__

#include "dataManager.h"
#include "orderBook.h"
// #define debugSM

class simulatorManager{
public:
	simulatorManager(orderBook*, dataManager* , unordered_map<string,DataSt> &,string &);
	// void loadExpiryFile(string);
	bool checkStrategyData(bool,vector<int> &);
	uint8_t getDatafromDM(long long &);
	void convertOrderbookMsgToDataStMsg();

private:
	string *data;
	unordered_map<string,DataSt> *dataM;
	dataManager* DMobj;
	orderBook* orderBookobj;
	string dataTyp;
	int modeAlready;
	int modeReq;
	string date;

	TradeMsgSt orderBookMsg;

};	

#endif
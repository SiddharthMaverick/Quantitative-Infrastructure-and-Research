#include <bits/stdc++.h>
#include "config.h"
#include "tradesDump.h"
#include "dataManager.h"
#include "simulatorManager.h"
#include "orderManager.h"
#include "strategyConnector.h"
#include "matchingManager.h"

// string dataFolderPath = "/home/data/testing_data/nsedata/";
string dataFolderPath = "/home/ubuntu/Simulator/data/";
string expiryfilepath = dataFolderPath+"expiryList.txt";
string lotsizefilepath = dataFolderPath+"historical_lot_sizes_01032016_20112020.csv";
string configfilepath;
string strategyconfigfilepath;

string CurSymbol;						//	Current Symbol feeded by DM from dataFiles
unordered_map<string,DataSt> DataMap;				//	Map between Current Symbol and its data e.g. like date, time, OHLC etc.
map <long long int, OrderSt> OrderMap;			//	Map between OrderID and its data e.g. symbolName, price, quantity etc.


int main(int argc, char *argv[])
{
	if(argc<3)
	{
		cout << "provide strategyConfigpath and configpath\n";
		return 0;
	}
	strategyconfigfilepath = argv[1];
	configfilepath = argv[2];

	if(!readConfigFile(configfilepath,dataFolderPath))				//	reading data from config.txt (function declared in config.cpp)
		return 0;

	dataManager* dataManagerOb;
	simulatorManager* simOb;
	strategyConnector* stOb;
	orderManager *orderManagerOb;
	matchingManager *matchingOb;
	tradesDump* tradesDumpOb;
	orderBook* orderBookOb;

	dataManagerOb = new dataManager(DataMap,CurSymbol, expiryfilepath,lotsizefilepath);				// DM object
	tradesDumpOb = new tradesDump(DataMap);
	matchingOb = new matchingManager(DataMap);								// MM object
	orderManagerOb = new orderManager(matchingOb,stOb,tradesDumpOb,orderBookOb,DataMap,OrderMap);	// OM object, giving MM and Strategy reference to it
	orderBookOb = new orderBook(orderManagerOb,DataMap);
	stOb = new strategyConnector(orderManagerOb,orderBookOb,strategyconfigfilepath);							// Strategy object, giving OM reference to it
	orderManagerOb->setStrategyAndBookOb(stOb,orderBookOb);
	simOb = new simulatorManager(orderBookOb, dataManagerOb,DataMap, CurSymbol);	// Simulator object, giving DM reference to it

	uint8_t getDataRet;
	long long curTime;

	while (getDataRet = simOb->getDatafromDM(curTime))	 // reading data from DM.. 0 if EOF, 1 if garbage line, 2 if read success (right symbol)
	{
		if(getDataRet==2)
		{
			tradesDumpOb->doAction(curTime);
			if(DataFreq == "SNAPSHOT")
				orderManagerOb->recData(CurSymbol);		
			else if(DataFreq == "MINUTELY")
				orderManagerOb->checkOrderFromQueue(CurSymbol);
//			else if(DataFreq == "MINUTELY" && MatchingBar > 0)
//				orderManagerOb->checkOrderFromPreviousBar(CurSymbol);

			stOb->onMarketData(DataMap[CurSymbol],CurSymbol);		// action on correct data
		}
	}	
	if(!InterDaysOn)
	{
		VolumeBasedMatching = false; 		// make Volume based Rule off
		orderManagerOb->giveTradeOfSquareOffOrders();		// give trade of pending square OFF order EOD
	}
	tradesDumpOb->dumpTradeInfoToFile();

	return 0;
}

#ifndef __Trades__
#define __Trades__

#include<bits/stdc++.h>
#include "config.h"
#include "structures.h"

//#define debugTradeDump

using namespace std;

class tradesDump{
public:
	tradesDump(unordered_map<string,DataSt> &);
	void doAction(long long);
	int getCurrentMinute(long long);
	void calcExposure(long long );
	bool checkMinuteIncrement(int);
	float getTransactionCost(string,long long,int,char);
	void recTradeDetails(TradeMsgSt);
	void tradeData();
	void calcPNL();
	long double calcMedian(vector<long long>);
	std::string getExposureData();
	void dumpTradeInfoToFile();
	// void getNetOrdersDataForAllSymbols(vector<std::string> &,vector<char> &, vector<int> &);
	// long long getMaxPNLOfCurSmybol(string);
	// long long getTotalPNL();
	// long long getCurSymbolPNL(string,int &);
	// long long getLastTradePNL(string,int &);
private:
	int curMinute = 0;
	int lastExposureTime = 0;
	unordered_map<string,DataSt> *dataMap;
	// vector<vector<string> > tradeDump;
	string tradeDumpFile;
	map<string,vector<long long> > pnl;
	map<string,TradesTotalSt> totalTradesData;				// contains trade data till now from morning/first Trade
	unordered_map<string,TradeMsgSt> lastTrade;				// it contains last Trade of every Symbol
	vector<long long> exposureArray;

	vector<string> vTemp = vector<string> (100);// contains comma seperated strings
	int index;// index iterator for above vector
	string substrTemp;
};

#endif

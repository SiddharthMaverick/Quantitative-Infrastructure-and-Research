#ifndef __structures__
#define __structures__

#include <bits/stdc++.h>

typedef struct DataSt{
	std::string date;
	long long int time;
	int open;
	int high;
	int low;
	int close;
	float volume;
	int OI;
	int lot;
	std::string expiry;
	int strike;
	std::string callput;
	int LTP;
	int LTQ;
	std::string LTT;
	int ATP;
	std::vector<std::pair<int,int> > buyDepth;
	std::vector<std::pair<int,int> > sellDepth;
	uint8_t messageType;
	long long int orderID;
	long long int orderID2;
	uint8_t side;
	long long int price;
	int quantity;
}DataSt;

typedef struct OrderSt{
	long long int orderID;
	std::string symbol;
	char side;
	int quantity;
	long long int price;
	char orderType;
	long long time;
	bool matchRemainingInNeXtBarOverRide;
}OrderSt;

typedef struct MatchedSt{

}MatchedSt;

typedef struct TradeMsgSt{
	long long int orderID;
	long long int orderID2;
	std::string symbol;
	char side;
	int quantity;
	long long int price;
	std::string date;
	long long int time;
	std::string expiry;
	int lot;
	char msgType;
}TradeMsgSt;

typedef struct TradesTotalSt{
	long long buyVal;
	long long sellVal;
	int buyQuant;
	int sellQuant;
	int buyTrades;
	int sellTrades;
	float buyTransactionCost;
	float sellTransactionCost;
}TradesTotalSt;

typedef struct priceLevel{
    int32_t quantity;                        /*** need to look if int32_t is enough for total quantity of all orders at that price ***/
    int32_t numOfOrders;
}priceLevel;

#endif

#ifndef __matchingManager__
#define __matchingManager__

#include <stdint.h>
#include "structures.h"
#include "config.h"

// #define debugMM

class matchingManager{
public:
	matchingManager(unordered_map<string,DataSt> &);
	char matchOrders(OrderSt , DataSt , TradeMsgSt &);
	char fillTradeDetails(TradeMsgSt &, long long int ,string ,char ,int ,long long int,long long int,string,string,int,int,bool);
private:
	unordered_map<string,DataSt>* dataMap;
	long long LTT = 0;
	int quantityTradeInCurBar = 0;
};

#endif

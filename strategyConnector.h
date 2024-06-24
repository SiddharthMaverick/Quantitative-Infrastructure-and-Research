#ifndef __strategyCon__
#define __strategyCon__

#include "orderManager.h"
#include "orderBook.h"
//#define debugStrat

class orderBook;
class orderManager;
class strategyConnector{
public:
	strategyConnector(orderManager *,orderBook*,string);

public:
   void readStrategyConfig(string);
   void sendOrder(char , int , int, string, char, long long,bool );
	bool onMarketData(DataSt, string);
	void onOrderResponse(char , long long int, string, char, int,int);

private:
	orderManager *orderObj;
   orderBook *bookObj;
   

};
#endif

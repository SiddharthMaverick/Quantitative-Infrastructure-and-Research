#ifndef __orderManager__
#define __orderManager__

#include "tradesDump.h"
#include "matchingManager.h"
#include "orderBook.h"
#include "strategyConnector.h"

// #define debugOM

class strategyConnector;
class orderBook;
class orderManager{
public:
	orderManager(matchingManager*,strategyConnector*,tradesDump*,orderBook*,unordered_map<string,DataSt> &, map <long long int, OrderSt> &);
	void setStrategyAndBookOb(strategyConnector*,orderBook*);
	void printOrder(OrderSt,string);
	void deleteOrderInSymbolMap(OrderSt);
	void insertOrderInSymbolMap(vector< pair<long long int,long long int> >,OrderSt);
	void checkOrderFromQueue(string);
	void recData(string);
	void cancelAllOrders(OrderSt);
	void recOrder(OrderSt Order, string);
	char sendToMatch(OrderSt, DataSt);
	void orderBookResponseTBT(std::string, char , long long , int , char , long long,long long ,long long );
	bool checkValidOrder(OrderSt,DataSt);
	bool checkInternalOrderCross(OrderSt);
	void giveTradeOfSquareOffOrders();

private:
	matchingManager* matchingOb;
	strategyConnector* stOb;
	tradesDump* tradesDOb;
	orderBook* orderBOb;
	map <long long int, OrderSt>* orderMap;
	unordered_map<string,DataSt>* dataMap;
	map<string,vector< pair<long long int,long long int> > > symbolToBuyOrdID;
	map<string,vector< pair<long long int,long long int> > > symbolToSellOrdID;
	TradeMsgSt Trade;
	char delimiter;

	// unordered_map<string,OrderSt> symbolToOrderID;								// created for next bar matching
	unordered_map<string,queue<OrderSt> > ordersQueue;						// created for orders of Bar data, which are not traded
};

#endif

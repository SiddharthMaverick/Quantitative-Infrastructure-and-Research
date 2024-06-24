#ifndef __orderbook__
#define __orderbook__

#include "structures.h"
#include "orderManager.h"

 #define debugBook

class orderManager;
class orderBook{
public:
    orderBook(orderManager*,unordered_map<string,DataSt> &);
    void insertOrderInBook(std::string,DataSt);
    void recIOCData(OrderSt);
    void recOrderData(OrderSt);      // overloaded function
    // void recOrderData(tradeMsgSt);
    bool newOrder(std::string,int32_t,int32_t,uint8_t,long long);
    void cancelOrder(std::string,int32_t,int32_t,uint8_t,long long);
    void modifyOrder(std::string,int32_t,int32_t,int32_t,int32_t,uint8_t,long long);
    void checkBookCross(OrderSt);
    void uncrossBook(std::string,char,long long,char);
    bool checkOrderBookMsg(TradeMsgSt &);
    void addMessage(std::string , char , long long , int , char , long long ,long long,long long);
    void getTopFromBook(std::string,int,std::vector<long long> &,std::vector<long long> &);
    void print();
    void printSnapshot();
    // void dumpBook();

private:
    // int32_t bookSize;

    private:
    orderManager* orderManagerObj;

    std::unordered_map<long long,OrderSt> orderMap;             // orderID to Structure map
    std::unordered_map<std::string,std::map<int32_t,std::vector<std::pair<long long,long long> >,std::greater<int32_t> > >symbolToBuyBook; 
    // map from symbol to BuyMap (map<symbol,map>). Buy Map contains decreasing order of price as key and priceLevel(structure) as Value
    std::unordered_map<std::string,std::map<int32_t,std::vector<std::pair<long long,long long> > > >symbolToSellBook;
    
    std::unordered_map<long long,OrderSt> ::iterator orderMapIter;    
    std::unordered_map<std::string,std::map<int32_t,std::vector<std::pair<long long,long long> >,std::greater<int32_t> > > ::iterator symbolToBuyBookIter; 
    std::unordered_map<std::string,std::map<int32_t,std::vector<std::pair<long long,long long> > > >::iterator symbolToSellBookIter;
    
    std::map<int32_t,std::vector<std::pair<long long,long long> >,std::greater<int32_t> > ::iterator innerBuyMapIter; 
    std::map<int32_t,std::vector<std::pair<long long,long long> > >::iterator innerSellMapIter;   

    long long topBuyPrice = 0;
    long long topSellPrice = LLONG_MAX;

    std::queue<TradeMsgSt> messages;

    long long lastExchangeTime = 0;

    unordered_map<string,DataSt> *dataMap;
};

#endif
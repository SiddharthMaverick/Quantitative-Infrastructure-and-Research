#include "simulatorManager.h"

simulatorManager::simulatorManager(orderBook* obOj, dataManager* dOj, unordered_map<string,DataSt> &dm, string &dataTemp)
{
	data = &dataTemp;				//	Current Symbol
	dataM = &dm;					//	DataMap's address
	DMobj = dOj;					//	DM object reference
	orderBookobj = obOj;			//	OB object reference
}

bool simulatorManager::checkStrategyData(bool getDataSuccess,vector<int> & dumy)
{
	return true;
}

uint8_t simulatorManager::getDatafromDM(long long &temp)
{
	if(DataFreq == "TBT" && orderBookobj->checkOrderBookMsg(orderBookMsg))	// check if ordeBook has to send message To Strategy(Trade message etc.)
	{
		convertOrderbookMsgToDataStMsg();		//	convert orderMsg to DataMsg which is read by strat.
		return 2;								//	2 means read data is Succes and Send data to Strategy
	}

	uint8_t retVar = DMobj->fetchData(temp);	// reading data from DM.. 0 if EOF, 1 if garbage line, 2 if read success (right symbol)
	
	if(DataFreq == "TBT" && retVar == 2)		// if TBT && if readData from Dm is success
	{
		orderBookobj->insertOrderInBook(*data,(*dataM)[*data]);			// symbol,dataMap[symbol]	insert in orderbook and 
		if(orderBookobj->checkOrderBookMsg(orderBookMsg))				// get msg from orderBook to pass on to Strat
		{
			convertOrderbookMsgToDataStMsg();
			return retVar;
		}
		else
			return 1;							//	no need to send data To strat as it was an IOC order which didnt get Traded in orderBook
	}
	
	return retVar;
}

void simulatorManager::convertOrderbookMsgToDataStMsg()
{
	DataSt tempDS = (*dataM)[orderBookMsg.symbol];		// to get lot out of current data	
	tempDS.messageType = orderBookMsg.msgType;		// N,M,X,T
	tempDS.time = orderBookMsg.time;
	tempDS.orderID = orderBookMsg.orderID;
	tempDS.orderID2 = orderBookMsg.orderID2;
	tempDS.side = orderBookMsg.side;
	tempDS.price = orderBookMsg.price;
	tempDS.quantity = orderBookMsg.quantity;

	(*dataM)[orderBookMsg.symbol] = tempDS;			// inserting data to DataMap
}

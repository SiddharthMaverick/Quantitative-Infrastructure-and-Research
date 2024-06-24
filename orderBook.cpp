#include "orderBook.h"
int counter = 1;
orderBook::orderBook(orderManager* omobj,unordered_map<string,DataSt> &dataTemp)
{
	orderManagerObj = omobj;
	dataMap = &dataTemp;
}

void orderBook::insertOrderInBook(std::string symbol,DataSt curData)		// this function converts DataSt to OrderSt(req by orderBook)
{
	OrderSt Order;
	Order.symbol = symbol;
	Order.orderType = curData.messageType;
	Order.time = curData.time;
	Order.orderID = curData.orderID;
	Order.side = curData.side;
	Order.price = curData.price;
	Order.quantity = curData.quantity;

	lastExchangeTime = curData.time;

	if(curData.messageType == 'I')
		recIOCData(Order);
	else
		recOrderData(Order);
}

void orderBook::recIOCData(OrderSt Order)
{
		/*** we can create a map of topBuyPrice and topSellPrice which will create top values of LevelMap at any time ***/
	#ifdef debugBook
	std::cout << "IOC\n";
	#endif
	if(Order.side == 'B')
	{
		long long px = Order.price;						
		long long int qt = Order.quantity;		

		symbolToSellBookIter = symbolToSellBook.find(Order.symbol);
		if(symbolToSellBookIter == symbolToSellBook.end())
			return;
		innerSellMapIter = symbolToSellBookIter->second.begin();
		
		while(symbolToSellBookIter->second.size() > 0 && px >= innerSellMapIter->first && qt > 0 )
		{
			if(innerSellMapIter->second[0].second > qt)
			{
				#ifdef debugBook
				std::cout << "S1trade of symbol " << Order.symbol <<" is generated at " << innerSellMapIter->first << " quant - " << qt << "\n";
				#endif
				//	add Trade... call trade Function
				addMessage(Order.symbol, Order.side, innerSellMapIter->first, qt, 'T', Order.time, Order.orderID, innerSellMapIter->second[0].first);
				(*dataMap)[Order.symbol].LTP = innerSellMapIter->first;
				innerSellMapIter->second[0].second -= qt;
				qt = 0;
			}
			else if(innerSellMapIter->second[0].second < qt)
			{
				#ifdef debugBook
				std::cout << "S2trade of symbol " << Order.symbol <<" is generated at " << innerSellMapIter->first << " quant - " << innerSellMapIter->second[0].second << "\n";
				#endif
				//	add Trade... call trade Function
				addMessage(Order.symbol, Order.side, innerSellMapIter->first, innerSellMapIter->second[0].second, 'T', Order.time,Order.orderID, innerSellMapIter->second[0].first);
				(*dataMap)[Order.symbol].LTP = innerSellMapIter->first;
				qt -= innerSellMapIter->second[0].second;
				orderMap.erase(innerSellMapIter->second[0].first);		// erase order from Map too
				innerSellMapIter->second.erase(innerSellMapIter->second.begin());		// delete the first element in the vector which got traded
				if(innerSellMapIter->second.size()==0)
				{
					symbolToSellBookIter->second.erase(innerSellMapIter);
					innerSellMapIter = symbolToSellBookIter->second.begin();	// if top level is Traded, point towards new top
				}
			}
			else
			{
				#ifdef debugBook
				std::cout << "S3trade of symbol " << Order.symbol <<" is generated at " << innerSellMapIter->first << " quant - " << qt << "\n";
				#endif
				//	add Trade... call trade Function
				addMessage(Order.symbol, Order.side, innerSellMapIter->first, qt, 'T', Order.time,Order.orderID, innerSellMapIter->second[0].first);
				(*dataMap)[Order.symbol].LTP = innerSellMapIter->first;
				qt = 0;
				orderMap.erase(innerSellMapIter->second[0].first);		// erase order from Map too
				innerSellMapIter->second.erase(innerSellMapIter->second.begin());		// delete the first element in the vector which got traded
				if(innerSellMapIter->second.size()==0)
					symbolToSellBookIter->second.erase(innerSellMapIter);
			}
		}
	}
	else if(Order.side == 'S')
	{
		long long px = Order.price;						
		long long int qt = Order.quantity;	
		
		symbolToBuyBookIter = symbolToBuyBook.find(Order.symbol);
		if(symbolToBuyBookIter == symbolToBuyBook.end())
			return;
		innerBuyMapIter = symbolToBuyBookIter->second.begin();
		
		while(symbolToBuyBookIter->second.size() > 0 && px <= innerBuyMapIter->first && qt > 0 )
		{
			if(innerBuyMapIter->second[0].second > qt)
			{
				#ifdef debugBook
				std::cout << "S1trade of symbol " << Order.symbol <<" is generated at " << innerBuyMapIter->first << " quant - " << qt << "\n";
				#endif
				//	add Trade... call trade Function
				addMessage(Order.symbol, Order.side, innerBuyMapIter->first, qt, 'T', Order.time,innerBuyMapIter->second[0].first, Order.orderID);
				(*dataMap)[Order.symbol].LTP = innerBuyMapIter->first;

				innerBuyMapIter->second[0].second -= qt;
				qt = 0;	
			}
			else if(innerBuyMapIter->second[0].second < qt)
			{
				#ifdef debugBook
				std::cout << "S2trade of symbol " << Order.symbol <<" is generated at " << innerBuyMapIter->first << " quant - " << innerBuyMapIter->second[0].second << "\n";
				#endif
				//	add Trade... call trade Function
				addMessage(Order.symbol, Order.side, innerBuyMapIter->first, innerBuyMapIter->second[0].second, 'T', Order.time,innerBuyMapIter->second[0].first, Order.orderID);
				(*dataMap)[Order.symbol].LTP = innerBuyMapIter->first;
				qt -= innerBuyMapIter->second[0].second;

				orderMap.erase(innerBuyMapIter->second[0].first);		// erase order from Map too
				innerBuyMapIter->second.erase(innerBuyMapIter->second.begin());		// delete the first element in the vector which got traded
				if(innerBuyMapIter->second.size()==0)
				{
					symbolToBuyBookIter->second.erase(innerBuyMapIter);
					innerBuyMapIter = symbolToBuyBookIter->second.begin();	// if top level is Traded, point towards new top
				}

			}
			else
			{
				#ifdef debugBook
				std::cout << "S3trade of symbol " << Order.symbol <<" is generated at " << innerBuyMapIter->first << " quant - " << qt << "\n";
				#endif
				//	add Trade... call trade Function
				addMessage(Order.symbol, Order.side, innerBuyMapIter->first, qt, 'T', Order.time,innerBuyMapIter->second[0].first, Order.orderID);
				(*dataMap)[Order.symbol].LTP = innerBuyMapIter->first;
				qt = 0;
	
				orderMap.erase(innerBuyMapIter->second[0].first);		// erase order from Map too
				innerBuyMapIter->second.erase(innerBuyMapIter->second.begin());		// delete the first element in the vector which got traded
				if(innerBuyMapIter->second.size()==0)
					symbolToBuyBookIter->second.erase(innerBuyMapIter);
			}
		}
	}
	#ifdef debugBook
	// print();
	printSnapshot();
	#endif
}

void orderBook::recOrderData(OrderSt Order)
{
	#ifdef debugBook
	std::cout << " calling orderBook orderMsg ";
	#endif
	orderMapIter = orderMap.find(Order.orderID);
	if(Order.orderType == 'M')
	{
		if(orderMapIter == orderMap.end())						// if modify order is not there in the Map
		{
			if(newOrder(Order.symbol,Order.price,Order.quantity,Order.side,Order.orderID))	// calling newOrder
			{
				Order.orderType = 'N';					// if that modify order was not in our book treat it at New
				orderMap[Order.orderID] = Order;		
				checkBookCross(Order);
			}
		}
		else
		{
			modifyOrder(Order.symbol,orderMapIter->second.price,orderMapIter->second.quantity,Order.price,Order.quantity,Order.side,Order.orderID);
			orderMap[Order.orderID] = Order;					// updating in OrderMap
			checkBookCross(Order);
		}
	}
	else if(Order.orderType == 'N')
	{
		if(orderMapIter != orderMap.end())						// if that new order is already there  in map 
			return;
		if(newOrder(Order.symbol,Order.price,Order.quantity,Order.side,Order.orderID))
		{
			orderMap[Order.orderID] = Order;		
			checkBookCross(Order);
		}
	}
	else if(Order.orderType == 'X')
	{
		if(orderMapIter == orderMap.end())						// if its not there in orderMap we cannot delete it
			return;
		cancelOrder(orderMapIter->second.symbol,orderMapIter->second.price,orderMapIter->second.quantity,orderMapIter->second.side,Order.orderID);
		orderMap.erase(Order.orderID);
		addMessage(Order.symbol,Order.side, Order.price, Order.quantity, Order.orderType, Order.time,Order.orderID,0);
	}
	#ifdef debugBook
	// print();
	printSnapshot();
	#endif
}
/*
void orderBook::recOrderData(tradeMsgSt Trade)							// overloaded function
{
	#ifdef debugBook
	std::cout << " calling orderBook Trademsg ";
	#endif
	orderMapIter = orderMap.find(Trade.buyOrderId);
	if(orderMapIter != orderMap.end())                      // if its not there in orderMap we cannot delete it
	{
		#ifdef debugBook
		std::cout << " buyTrade ";
		#endif
		if(orderMapIter->second.quantity == Trade.tradeQuantity)
		{
			cancelOrder(orderMapIter->second.symbol,orderMapIter->second.price,orderMapIter->second.quantity,orderMapIter->second.side);
			orderMap.erase(Trade.buyOrderId);
		}
		else if(orderMapIter->second.quantity > Trade.tradeQuantity)
		{
			modifyOrder(Trade.symbol,orderMapIter->second.price,orderMapIter->second.quantity,orderMapIter->second.price,orderMapIter->second.quantity - Trade.tradeQuantity,'B');
			orderMapIter->second.quantity = orderMapIter->second.quantity - Trade.tradeQuantity;					// updating in OrderMap
		}
	}

	orderMapIter = orderMap.find(Trade.sellOrderId);

	if(orderMapIter != orderMap.end())                      // if its not there in orderMap we cannot delete it
	{
		#ifdef debugBook
		std::cout << " sellTrade ";
		#endif
		if(orderMapIter->second.quantity == Trade.tradeQuantity)
		{
			cancelOrder(orderMapIter->second.symbol,orderMapIter->second.price,orderMapIter->second.quantity,orderMapIter->second.side);
			orderMap.erase(Trade.sellOrderId);
		}
		else if(orderMapIter->second.quantity > Trade.tradeQuantity)
		{
			modifyOrder(Trade.symbol,orderMapIter->second.price,orderMapIter->second.quantity,orderMapIter->second.price,orderMapIter->second.quantity - Trade.tradeQuantity,'S');
			orderMapIter->second.quantity = orderMapIter->second.quantity - Trade.tradeQuantity;					// updating in OrderMap
		}
	}
	#ifdef debugBook
//	print();	
	#endif
}
*/
bool orderBook::newOrder(std::string symbol, int32_t price,int32_t quant,uint8_t side,long long int ordID)
{
	if(side == 'B')
	{
		#ifdef debugBook
		std::cout << "new Buy" << std::endl;
		#endif
		symbolToBuyBookIter = symbolToBuyBook.find(symbol);
		if(symbolToBuyBookIter == symbolToBuyBook.end())					// if current symbol is not there in map
		{
			std::map<int32_t,std::vector<std::pair<long long,long long> >,std::greater<int32_t> > levelBook;
			symbolToBuyBookIter = symbolToBuyBook.insert({symbol,levelBook}).first;		// iterator towards current symbol's values (<key,value>)
//			symbolToBuyBook[symbol] = levelBook;
//			symbolToBuyBookIter = symbolToBuyBook.find(symbol);
		}

		innerBuyMapIter = symbolToBuyBookIter->second.find(price);				// get iterator towards map of map
		if(innerBuyMapIter != symbolToBuyBookIter->second.end())
		{
			/*** insert Order (make pair and insert at vector end) at end ***/
			innerBuyMapIter->second.push_back(std::make_pair(ordID,quant));

			// innerBuyMapIter->second.quantity += quant;
			// innerBuyMapIter->second.numOfOrders++;
		}
		else
		{
			/*** create new vector and insert pair inside it 
			symbolToBuyBookIter->second[price] = new pair which we made; ***/
			std::vector<std::pair<long long,long long> >levelVec;
			levelVec.push_back(std::make_pair(ordID,quant));
			symbolToBuyBookIter->second[price] = levelVec;

			// priceLevel level;											// cearing new level
			// level.quantity = quant;
			// level.numOfOrders = 1;
			// symbolToBuyBookIter->second[price] = level;

			// if(symbolToBuyBookIter->second.size() > bookSize)								// if levels become > bookSize
			// {

			// 	symbolToBuyBookIter->second.erase(symbolToBuyBookIter->second.rbegin()->first);		// delete last element in that levelBook

			// 	if(symbolToBuyBookIter->second.rbegin()->first > price)	// if newOrder does not belong to top Book, then do do any Operation
			// 		return false;												// if last element of sorted(decr.)	is greater then new price order then ignore
			// }

		}

	}
	else if(side == 'S')
	{
		#ifdef debugBook
		std::cout << "new Sell"<< std::endl;
		#endif
		symbolToSellBookIter = symbolToSellBook.find(symbol);
		if(symbolToSellBookIter == symbolToSellBook.end())
		{
			std::map<int32_t,std::vector<std::pair<long long,long long> > > levelBook;
			symbolToSellBookIter = symbolToSellBook.insert({symbol,levelBook}).first;
		//	symbolToSellBook[symbol] = levelBook;
		//	symbolToSellBookIter = symbolToSellBook.find(symbol);
		}
		// std::cout << "1\n";
		innerSellMapIter = symbolToSellBookIter->second.find(price);
		// std::cout << "2\n";
		if(innerSellMapIter != symbolToSellBookIter->second.end())
		{
			innerSellMapIter->second.push_back(std::make_pair(ordID,quant));
		}
		else
		{
			std::vector<std::pair<long long,long long> >levelVec;
			levelVec.push_back(std::make_pair(ordID,quant));
			symbolToSellBookIter->second[price] = levelVec;

			// priceLevel level;											// cearing new level
			// level.quantity = quant;
			// level.numOfOrders = 1;
			// symbolToSellBookIter->second[price] = level;

			// if(symbolToSellBookIter->second.size() > bookSize)								// if levels become > 100
			// {

			// 	symbolToSellBookIter->second.erase(symbolToSellBookIter->second.rbegin()->first);

			// 	if(symbolToSellBookIter->second.rbegin()->first < price)	// if newOrder does not belong to top Book, then do do any Operation
			// 		return false;												
			// }

		}

	}

	return true;
}

void orderBook::cancelOrder(std::string symbol, int32_t price,int32_t quant,uint8_t side,long long int ordID)
{
	if(side == 'B')
	{
		#ifdef debugBook
		std::cout << "calcel Buy"<< std::endl;
		#endif
		symbolToBuyBookIter = symbolToBuyBook.find(symbol);
//		if(innerBuyMapIter == symbolToBuyBookIter->second.end())
//			return;
		innerBuyMapIter = symbolToBuyBookIter->second.find(price);
		if(innerBuyMapIter == symbolToBuyBookIter->second.end())
			return;

		std::vector<std::pair<long long,long long> >levelVec = innerBuyMapIter->second;
		for(int i=0;i<innerBuyMapIter->second.size();i++)
			if(innerBuyMapIter->second[i].first == ordID)
			{
				innerBuyMapIter->second.erase(innerBuyMapIter->second.begin()+i);
				break;
			}

		// innerBuyMapIter->second.quantity -= quant;
		// innerBuyMapIter->second.numOfOrders--;

		if(innerBuyMapIter->second.empty())							// if that level contains not pair after deleteing order, then remove that level from map
			symbolToBuyBookIter->second.erase(innerBuyMapIter);
	}
	else if(side == 'S')
	{
		#ifdef debugBook
		std::cout << "calcel Sell"<< std::endl;
		#endif
		symbolToSellBookIter = symbolToSellBook.find(symbol);
//		if(innerSellMapIter == symbolToSellBookIter->second.end())
//			return;
		innerSellMapIter = symbolToSellBookIter->second.find(price);
		if(innerSellMapIter == symbolToSellBookIter->second.end())
			return;

		std::vector<std::pair<long long,long long> >levelVec = innerSellMapIter->second;
		// innerSellMapIter->second.quantity -= quant;
		// innerSellMapIter->second.numOfOrders--;
		for(int i=0;i<innerSellMapIter->second.size();i++)
			if(innerSellMapIter->second[i].first == ordID)
			{
				innerSellMapIter->second.erase(innerSellMapIter->second.begin()+i);
				break;
			}

		if(innerSellMapIter->second.empty())							// if that level contains 0 quantity after deleteing order, then remove that level from map
			symbolToSellBookIter->second.erase(innerSellMapIter);	
	}


}

void orderBook::modifyOrder(std::string symbol, int32_t prevprice,int32_t prevquant,int32_t price,int32_t quant,uint8_t side,long long int ordID)
{			/*** assuming side would not change in modify orders ***/
	#ifdef debugBook
	std::cout << "modify"<< std::endl;
	#endif
	if(price != prevprice ||  prevquant < quant)
	{
		cancelOrder(symbol,prevprice,prevquant,side,ordID);						/*** calling symbol to book map twice ***/
		newOrder(symbol,price,quant,side,ordID);									/*** once above and once here ***/
		return;
	}

	/*** if new quantity is greater than prev quantity treat is as cancel and New ***/

	if(side == 'B')
	{	
		innerBuyMapIter = symbolToBuyBook[symbol].find(price);
		for(int i=0;i<innerBuyMapIter->second.size();i++)
			if(innerBuyMapIter->second[i].first == ordID)
			{
				innerBuyMapIter->second[i].second = quant;
				break;
			}

	}
	else if(side == 'S')
	{
		innerSellMapIter = symbolToSellBook[symbol].find(price);
		for(int i=0;i<innerSellMapIter->second.size();i++)
			if(innerSellMapIter->second[i].first == ordID)
			{
				innerSellMapIter->second[i].second = quant;						// modifying quantity in either sell or buy map
				break;
			}
	}
}

void orderBook::checkBookCross(OrderSt order)
{
	symbolToBuyBookIter = symbolToBuyBook.find(order.symbol);
	symbolToSellBookIter = symbolToSellBook.find(order.symbol);
	if(symbolToBuyBookIter != symbolToBuyBook.end() && symbolToSellBookIter != symbolToSellBook.end())	// check if symbols are there in book
		if(symbolToBuyBookIter->second.size() > 0 && symbolToSellBookIter->second.size() > 0 && symbolToBuyBookIter->second.begin()->first >= symbolToSellBookIter->second.begin()->first)
		{	// check if inner price level Maps have atleast 1 element and if yes compare topBuy>=topSell
			uncrossBook(order.symbol,order.side,order.time,order.orderType);	
			return;
		}

	addMessage(order.symbol,order.side, order.price, order.quantity, order.orderType, order.time,order.orderID,0);
}

void orderBook::uncrossBook(std::string symbol,char side,long long time,char orderType)
{
	/*** we can create a map of topBuyPrice and topSellPrice which will create top values of LevelMap at any time ***/
	#ifdef debugBook
	std::cout << "uncrossing"<< std::endl;
	#endif
	if(side == 'B')
	{
		symbolToBuyBookIter = symbolToBuyBook.find(symbol);
		innerBuyMapIter = symbolToBuyBookIter->second.begin();		// get top of BuyLevelMap
		long long px = innerBuyMapIter->first;						// get top price in Buy
		long long int qt = innerBuyMapIter->second[0].second;		// first element of that vector

		symbolToSellBookIter = symbolToSellBook.find(symbol);
		innerSellMapIter = symbolToSellBookIter->second.begin();
		
		while(symbolToSellBookIter->second.size() > 0 && px >= innerSellMapIter->first && qt > 0 )
		{
			if(innerBuyMapIter->second[0].first>=OrderIDStartingNumber && innerSellMapIter->second[0].first>=OrderIDStartingNumber)
			{	//internalOrderCrossing
				addMessage(symbol, side, innerBuyMapIter->first, qt, 'X', time,innerBuyMapIter->second[0].first,0);
				orderMap.erase(innerBuyMapIter->second[0].first);		// erase order from Map too
				symbolToBuyBookIter->second.erase(innerBuyMapIter);		// as that level had only 1 order(newest Order which came)
				return;
			}

			if(innerSellMapIter->second[0].second > qt)
			{
				#ifdef debugBook
				std::cout << "S1trade of symbol " << symbol <<" is generated at " << innerSellMapIter->first << " quant - " << qt << "\n";
				#endif
				//	add Trade... call trade Function
				addMessage(symbol, side, innerSellMapIter->first, qt, 'T', time, innerBuyMapIter->second[0].first, innerSellMapIter->second[0].first);
				(*dataMap)[symbol].LTP = innerSellMapIter->first;
				
				innerSellMapIter->second[0].second -= qt;
				qt = 0;
				orderMap.erase(innerBuyMapIter->second[0].first);		// erase order from Map too
				symbolToBuyBookIter->second.erase(innerBuyMapIter);		// as that level had only 1 order(newest Order which came)
			}
			else if(innerSellMapIter->second[0].second < qt)
			{
				#ifdef debugBook
				std::cout << "S2trade of symbol " << symbol <<" is generated at " << innerSellMapIter->first << " quant - " << innerSellMapIter->second[0].second << "\n";
				#endif
				//	add Trade... call trade Function
				addMessage(symbol, side, innerSellMapIter->first, innerSellMapIter->second[0].second, 'T', time,innerBuyMapIter->second[0].first, innerSellMapIter->second[0].first);
				(*dataMap)[symbol].LTP = innerSellMapIter->first;

				qt -= innerSellMapIter->second[0].second;
				orderMap.erase(innerSellMapIter->second[0].first);		// erase order from Map too
				innerSellMapIter->second.erase(innerSellMapIter->second.begin());		// delete the first element in the vector which got traded
				if(innerSellMapIter->second.size()==0)
				{
					symbolToSellBookIter->second.erase(innerSellMapIter);
					innerSellMapIter = symbolToSellBookIter->second.begin();	// if top level is Traded, point towards new top
				}
			}
			else
			{
				#ifdef debugBook
				std::cout << "S3trade of symbol " << symbol <<" is generated at " << innerSellMapIter->first << " quant - " << qt << "\n";
				#endif
				//	add Trade... call trade Function
				addMessage(symbol, side, innerSellMapIter->first, qt, 'T', time,innerBuyMapIter->second[0].first, innerSellMapIter->second[0].first);
				(*dataMap)[symbol].LTP = innerSellMapIter->first;
				
				qt = 0;
				orderMap.erase(innerBuyMapIter->second[0].first);		// erase order from Map too
				symbolToBuyBookIter->second.erase(innerBuyMapIter);		// as that level had only 1 order(newest Order which came)
				
				orderMap.erase(innerSellMapIter->second[0].first);		// erase order from Map too
				innerSellMapIter->second.erase(innerSellMapIter->second.begin());		// delete the first element in the vector which got traded
				if(innerSellMapIter->second.size()==0)
					symbolToSellBookIter->second.erase(innerSellMapIter);
			}
		}
		if(qt>0)										
		{
			innerBuyMapIter->second[0].second = qt;
			addMessage(symbol, side, innerBuyMapIter->first, qt, orderType, time,innerBuyMapIter->second[0].first,0);
		}
	}
	else if(side == 'S')
	{
		symbolToSellBookIter = symbolToSellBook.find(symbol);
		innerSellMapIter = symbolToSellBookIter->second.begin();		// get top of SellLevelMap
		long long px = innerSellMapIter->first;						// get top price in Sell
		long long int qt = innerSellMapIter->second[0].second;		// first element of that vector
		
		symbolToBuyBookIter = symbolToBuyBook.find(symbol);
		innerBuyMapIter = symbolToBuyBookIter->second.begin();
		
		while(symbolToBuyBookIter->second.size() > 0 && px <= innerBuyMapIter->first && qt > 0 )
		{
			if(innerBuyMapIter->second[0].first>=OrderIDStartingNumber && innerSellMapIter->second[0].first>=OrderIDStartingNumber)
			{	//internalOrderCrossing
				addMessage(symbol, side, innerSellMapIter->first, qt, 'X', time,innerSellMapIter->second[0].first,0);
				orderMap.erase(innerSellMapIter->second[0].first);		// erase order from Map too
				symbolToSellBookIter->second.erase(innerSellMapIter);		// as that level had only 1 order(newest Order which came)
				return;
			}
			if(innerBuyMapIter->second[0].second > qt)
			{
				#ifdef debugBook
				std::cout << "S1trade of symbol " << symbol <<" is generated at " << innerBuyMapIter->first << " quant - " << qt << "\n";
				#endif
				//	add Trade... call trade Function
				addMessage(symbol, side, innerBuyMapIter->first, qt, 'T', time,innerBuyMapIter->second[0].first, innerSellMapIter->second[0].first);
				(*dataMap)[symbol].LTP = innerBuyMapIter->first;

				innerBuyMapIter->second[0].second -= qt;
				qt = 0;	
				orderMap.erase(innerSellMapIter->second[0].first);		// erase order from Map too
				symbolToSellBookIter->second.erase(innerSellMapIter);		// as that level had only 1 order(newest Order which came)
			}
			else if(innerBuyMapIter->second[0].second < qt)
			{
				#ifdef debugBook
				std::cout << "S2trade of symbol " << symbol <<" is generated at " << innerBuyMapIter->first << " quant - " << innerBuyMapIter->second[0].second << "\n";
				#endif
				//	add Trade... call trade Function
				addMessage(symbol, side, innerBuyMapIter->first, innerBuyMapIter->second[0].second, 'T', time,innerBuyMapIter->second[0].first, innerSellMapIter->second[0].first);
				(*dataMap)[symbol].LTP = innerBuyMapIter->first;

				qt -= innerBuyMapIter->second[0].second;
				orderMap.erase(innerBuyMapIter->second[0].first);		// erase order from Map too
				innerBuyMapIter->second.erase(innerBuyMapIter->second.begin());		// delete the first element in the vector which got traded
				if(innerBuyMapIter->second.size()==0)
				{
					symbolToBuyBookIter->second.erase(innerBuyMapIter);
					innerBuyMapIter = symbolToBuyBookIter->second.begin();	// if top level is Traded, point towards new top
				}

			}
			else
			{
				#ifdef debugBook
				std::cout << "S3trade of symbol " << symbol <<" is generated at " << innerBuyMapIter->first << " quant - " << qt << "\n";
				#endif
				//	add Trade... call trade Function
				addMessage(symbol, side, innerBuyMapIter->first, qt, 'T', time,innerBuyMapIter->second[0].first, innerSellMapIter->second[0].first);
				(*dataMap)[symbol].LTP = innerBuyMapIter->first;

				qt = 0;
				orderMap.erase(innerSellMapIter->second[0].first);		// erase order from Map too
				symbolToSellBookIter->second.erase(innerSellMapIter);		// as that level had only 1 order(newest Order which came)
	
				orderMap.erase(innerBuyMapIter->second[0].first);		// erase order from Map too
				innerBuyMapIter->second.erase(innerBuyMapIter->second.begin());		// delete the first element in the vector which got traded
				if(innerBuyMapIter->second.size()==0)
					symbolToBuyBookIter->second.erase(innerBuyMapIter);
			}
		}
		if(qt>0)
		{
			innerSellMapIter->second[0].second = qt;
			addMessage(symbol, side, innerSellMapIter->first, qt, orderType, time,innerSellMapIter->second[0].first,0);
		}
	}
}

void orderBook::addMessage(std::string symbol, char s, long long px, int qt, char mType, long long time,long long OID1,long long OID2)
{
	TradeMsgSt msg;
	msg.symbol = symbol;
	msg.side = s;
	msg.price = px;
	msg.quantity = qt;
	msg.msgType = mType;
	msg.time = time;
	msg.orderID = OID1;
	msg.orderID2 = OID2;

	if( OID1 >= OrderIDStartingNumber || OID2 >= OrderIDStartingNumber)
	{	
		if(mType == 'T' || mType == 'X') // if Trade happened of Strategy Order, send Trade info to OM
			orderManagerObj->orderBookResponseTBT(symbol,s,px,qt,mType,lastExchangeTime,OID1,OID2);
	}
	else
		messages.push(msg);		// push data in vector
}

bool orderBook::checkOrderBookMsg(TradeMsgSt &msg)
{
	if(messages.empty())		// if not Data is there in orderBook to send to Strat
		return false;

	msg = messages.front();
	messages.pop();	// erase data which is sent to orderBook
	return true;
}

void orderBook::getTopFromBook(std::string symbol,int numOfLevels,std::vector<long long> &topBuy,std::vector<long long> &topSell)
{
	topBuy.clear();topSell.clear();			// emptying both vectors before pushing values in them

	symbolToBuyBookIter = symbolToBuyBook.find(symbol);
	if(symbolToBuyBookIter != symbolToBuyBook.end())
	{
		innerBuyMapIter = symbolToBuyBookIter->second.begin();
		for(int i=0; i<numOfLevels && innerBuyMapIter != symbolToBuyBookIter->second.end() ;i++,innerBuyMapIter++)
			topBuy.push_back(innerBuyMapIter->first);
	}

	symbolToSellBookIter = symbolToSellBook.find(symbol);
	if(symbolToSellBookIter != symbolToSellBook.end())
	{
		innerSellMapIter = symbolToSellBookIter->second.begin();
		for(int i=0; i<numOfLevels && innerSellMapIter != symbolToSellBookIter->second.end() ;i++,innerSellMapIter++)
			topSell.push_back(innerSellMapIter->first);
	}
}

void orderBook::printSnapshot()
{
	std::cout << "\n\nBuy**********************************************************************************" << symbolToBuyBook.size() << "\n";
	for (const auto& x : symbolToBuyBook) 
	{
		std::cout << "symbol " << x.first << " " << x.second.size() << "\n";
		int cc=0;
		for (const auto i : x.second) 
		{
			std::cout << i.first << " " << i.second.size() << "\n";
			cc++;
			if(cc>=5)
				break;
			// for(int ii=0;ii<i.second.size();ii++)
				// std::cout << i.second[ii].first << ":" << i.second[ii].second << "  ";
		}
		std::cout<< std::endl;
	}


	std::cout << "\n\nSell**********************************************************************************" << symbolToSellBook.size() << "\n";
	for (const auto& x : symbolToSellBook) 
	{
		std::cout << "symbol " << x.first << " " << x.second.size() << "\n";
		int cc=0;
		for (const auto i : x.second) 
		{
			std::cout << i.first << " " << i.second.size() << "\n";
			cc++;
			if(cc>=5)
				break;
			// for(int ii=0;ii<i.second.size();ii++)
				// std::cout << i.second[ii].first << ":" << i.second[ii].second << "  ";
		}
		std::cout << std::endl;
	}
}

void orderBook::print()
{
	std::cout << "\n\nBuy**********************************************************************************" << symbolToBuyBook.size() << "\n";
	for (const auto& x : symbolToBuyBook) 
	{
		std::cout << "symbol " << x.first << " " << x.second.size() << "   ";
		for (const auto i : x.second) 
		{
			std::cout << i.first << ":";
			for(int ii=0;ii<i.second.size();ii++)
				std::cout << i.second[ii].first << ":" << i.second[ii].second << "  ";
		}
		std::cout << "\n";
	}


	std::cout << "\n\nSell**********************************************************************************" << symbolToSellBook.size() << "\n";
	for (const auto& x : symbolToSellBook) 
	{
		std::cout << "symbol " << x.first << " " << x.second.size() << "   ";
		for (const auto i : x.second) 
		{
			std::cout << i.first << ":";
			for(int ii=0;ii<i.second.size();ii++)
				std::cout << i.second[ii].first << ":" << i.second[ii].second << "  ";
		}
		std::cout << "\n";
	}
}

// void orderBook::dumpBook()
// {
// 	std::ofstream ofs1 ("orderBook1.txt", std::ofstream::out | std::ofstream::app );			// output file stream

// 	for(auto it : tokenToBuyBook)
// 	{
// 		ofs1 << it.first << " " << counter++ << " ***************************BUY- " << it.second.size() << " - ";
// 		for(auto it2 : it.second)
// 			ofs1 << it2.side << ":" << it2.price << ":" << it2.quantity << " , ";
// 		ofs1 << "\n";
// 	}
// 	ofs1 << "\n\n";

// 			ofs1 << it2.side << ":" << it2.price << ":" << it2.quantity << " , ";
// 		ofs1 << "\n";
// 	}
// 	ofs1 << "\n\n";

// 	std::ofstream ofs2 ("orderBook2.txt", std::ofstream::out | std::ofstream::app );			// output file stream
// 	for(auto it : tokenToSellBook)
// 	{
// 		ofs2 << it.first << " " << counter++ << " ***************************Sell- " << it.second.size() << " - ";
// 		for(auto it2 : it.second)
// 			ofs2 << it2.side << ":" << it2.price << ":" << it2.quantity << " , ";
// 		ofs2 << "\n";
// 	}

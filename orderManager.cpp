#include "orderManager.h"

orderManager::orderManager(matchingManager* ob,strategyConnector* st ,tradesDump* td,orderBook* OBobj,unordered_map<string,DataSt> &dataM, map <long long int, OrderSt> &orderM)
{	
	matchingOb = ob;			// MM object pointer
	orderMap = &orderM;			// ordermap declared in main.cpp
	tradesDOb = td;
	orderBOb = OBobj;
	dataMap = &dataM;
	stOb = st;					// strategy object
	delimiter = '|';

	for(auto it=validSymbols.begin();it!=validSymbols.end();it++)
		ordersQueue[*it] = queue<OrderSt>();

}

void orderManager::setStrategyAndBookOb(strategyConnector* st,orderBook* book)
{
	stOb = st;
	orderBOb = book;
}

void orderManager::printOrder(OrderSt Order,string symbl)
{
	cout << "Order Details: " << Order.symbol << " " << Order.orderID << " " << Order.side << " " << Order.quantity << " " 
	<< Order.price << " " << Order.orderType << " lot: " << (*dataMap)[symbl].lot << endl;
}

void orderManager::insertOrderInSymbolMap(vector< pair<long long int,long long int> >orderIDs, OrderSt Order)	// symbol->vector = OrderIDs, Order = need to be inserted 
{
	pair<long long int,long long int> temporary(Order.orderID,Order.price); 	// copying data from Order
	for(int i=0;i<orderIDs.size()+1;i++)									// finding position to insert
		if(i==orderIDs.size())												// if loop ends, insert at end 
		{
			orderIDs.insert(orderIDs.end(),temporary);						// Order.price is greatest among all availabe till now
			break;
		}
		else if(orderIDs[i].second > Order.price)								// if found a position for price in sorted vector (acc to price)
		{
			orderIDs.insert(orderIDs.begin()+i,temporary);					// insert at that position
			break;
		}

	if(Order.side == 'S')
		symbolToSellOrdID[Order.symbol] = orderIDs;									// update the map
	else 
		symbolToBuyOrdID[Order.symbol] = orderIDs;
}
		
void orderManager::deleteOrderInSymbolMap(OrderSt Order)					// Order to be Deleted
{
	vector< pair<long long int,long long int> > orderIDs;
	if(Order.side == 'S')
		orderIDs = symbolToSellOrdID[Order.symbol];							// getting symbol to OrderID map
	else
		orderIDs = symbolToBuyOrdID[Order.symbol];

	for(int i=0;i<orderIDs.size();i++)										// running loop to find OrderID to be deleted
		if(orderIDs[i].first == Order.orderID)									// if current orderID found
		{
			orderIDs.erase(orderIDs.begin() + i);							// removing that orderID from symbolTOordID Map
			break;
		}

	if(Order.side == 'S')
		if(orderIDs.empty())													// if no order available for that symbol
			symbolToSellOrdID.erase(Order.symbol);									// remove symbol from symbolToSellOrdID map
		else																	// else
			symbolToSellOrdID[Order.symbol] = orderIDs;								// update the map
	else
		if(orderIDs.empty())
			symbolToBuyOrdID.erase(Order.symbol);
		else
			symbolToBuyOrdID[Order.symbol] = orderIDs;
}

void orderManager::checkOrderFromQueue(string symbl)
{
	// if(symbolToOrderID.find(symbl)!=symbolToOrderID.end())
	// {
	// 	sendToMatch(symbolToOrderID[symbl],(*dataMap)[string(symbl)]);
	// 	symbolToOrderID.erase(symbl);
	// }
	auto it = ordersQueue.find(symbl);
	char tradeResponse;
    while(!it->second.empty() && (tradeResponse = sendToMatch(it->second.front(),(*dataMap)[symbl])) == 'T')
    	it->second.pop();

    if(tradeResponse == 'p')			// partially Traded and rest Rejected
		it->second.pop();
	else if(tradeResponse == 'P');	// partially traded and rest can be traded in next Bar
		it->second.front().quantity = (*orderMap)[it->second.front().orderID].quantity; 	// updated quantity from orderMap
}

void orderManager::recData(string curSymbol)
{
	if(symbolToSellOrdID.find(curSymbol)!=symbolToSellOrdID.end())		/*** calling find function 2 times, one here and one in next line ***/
	{
		vector< pair<long long int,long long int> > orderIDs = symbolToSellOrdID[curSymbol];	// get sell orderIDs of current symbol
		for(int i=0; i<orderIDs.size();i++)												// loop should run until matching happens
			if(sendToMatch((*orderMap)[orderIDs[i].first],(*dataMap)[curSymbol]) == '0')
				break;
	}																			/*** need only 1 side(b or s) for mathcing... put check condition ***/
	if(symbolToBuyOrdID.find(curSymbol) != symbolToBuyOrdID.end())
	{
		vector< pair<long long int,long long int> > orderIDs = symbolToBuyOrdID[curSymbol];		// get buy orderIDs of current symbol
		for(int i=orderIDs.size()-1; i>=0;i--)														// send "Sell" orders in increasing manner
			if(sendToMatch((*orderMap)[orderIDs[i].first],(*dataMap)[curSymbol]) == '0')
				break;
	}

}

void orderManager::cancelAllOrders(OrderSt order)
{
	auto it = ordersQueue.find(order.symbol);
	if(it->second.empty())
		stOb->onOrderResponse('R',order.orderID,order.symbol,order.side,order.quantity,order.price);	// if there is no pending order in bar Order's queue
	while(!it->second.empty())
	{
		OrderSt Order = it->second.front();
		(*orderMap).erase(Order.orderID);			// erase from orderMap
		stOb->onOrderResponse('X',Order.orderID,Order.symbol,Order.side,Order.quantity,Order.price);   // cancel order Response
		it->second.pop();
	}
}

void orderManager::recOrder(OrderSt Order, string symbl)
{
	DataSt Cur = (*dataMap)[string(symbl)];
	if(Order.orderType == 'X')											// Cancel order
	{																	/*** should be made similar to exchange ***/
		if(DataFreq == "MINUTELY")										// if its bar data, cancel orderType means, cancel all pending Orders
		{
			cancelAllOrders(Order);	
		}
		else if((*orderMap).find(Order.orderID) == (*orderMap).end())
		{
			#ifdef debugOM
			cout << "we failed to cancel order as it's already matched.\n";
			printOrder(Order,symbl);
			#endif

			stOb->onOrderResponse('R',Order.orderID,Order.symbol,Order.side,Order.quantity,Order.price);	// cancel orderID wasn't in map/ order was processed earlier
		}
		else
		{
			(*orderMap).erase(Order.orderID);
			
			if(DataFreq == "SNAPSHOT")											// only for SNAPSHOT
				deleteOrderInSymbolMap(Order);									// deleting from symbolToOrdID map

			#ifdef debugOM
			cout << "cancellation of order ID:" << Order.orderID << " Successful.\n";
			printOrder(Order,symbl);
			#endif

			if(DataFreq == "TBT")
				orderBOb->recOrderData(Order);
			stOb->onOrderResponse('X',Order.orderID,Order.symbol,Order.side,Order.quantity,Order.price);   // cancel order success response
		}
 	}
	else
	{
		if(Order.orderType == 'M')
			if((*orderMap).find(Order.orderID) == (*orderMap).end())
			{
				#ifdef debugOM
				cout << "we failed to modify order as it's already macthed.\n";
				printOrder(Order,symbl);
				#endif

				stOb->onOrderResponse('R',Order.orderID,Order.symbol,Order.side,Order.quantity,Order.price); // Order was proccessed and not in orderMap
			}
			else
			{
				if((*orderMap)[Order.orderID].symbol == Order.symbol && (*orderMap)[Order.orderID].side == Order.side && checkValidOrder(Order,Cur) && !checkInternalOrderCross(Order)) 
				{										// modifying only if price or quant changed also if checkOrder returns true and checkInternal returns false

					if(DataFreq == "SNAPSHOT")									// only for snapshot
					{
						OrderSt tempOrder = (*orderMap)[Order.orderID];					// tempOrder to compare price and quant of New to Old

						vector< pair<long long int,long long int> > orderIDs;
						if(Order.side == 'S') 
							orderIDs = symbolToSellOrdID[Order.symbol];	// get orderIDs from map
						else 
							orderIDs = symbolToBuyOrdID[Order.symbol];

						for(int i=0;i<orderIDs.size();i++)							// running loop over all orderIDs of Order.symbol to find exact orderID
							if(orderIDs[i].first == Order.orderID)						// if req. orderID found
							{
								if(tempOrder.price == Order.price && tempOrder.quantity > Order.quantity)
								{
									orderIDs[i].second = Order.quantity;
									if(Order.side == 'S') 
										symbolToSellOrdID[Order.symbol] = orderIDs; 		// put OrderID back in Map if quantity decreased and price is same
									else 
										symbolToBuyOrdID[Order.symbol] = orderIDs;
								}
								else
								{
									orderIDs.erase(orderIDs.begin() + i);				// removing that orderID from symbolTOordID Map
									insertOrderInSymbolMap(orderIDs,Order);						// insert in same orderIDs amd then to Map
																					// erasing because we cannot insert price at that place, as its price sorted
								}
								break;
							}

					}

					(*orderMap)[Order.orderID] = Order;								// modifying in OrderMap

					stOb->onOrderResponse('M',Order.orderID,Order.symbol,Order.side,Order.quantity,Order.price);

					if(DataFreq == "TBT")
						orderBOb->recOrderData(Order);
					else if(DataFreq == "MINUTELY")			// for next bar matching
					{
						// symbolToOrderID[symbl] = Order;
						auto it = ordersQueue.find(symbl);
						if(VolumeBasedMatching && !OrderMatchingInMultBars && !it->second.empty() && it->second.front().matchRemainingInNeXtBarOverRide)
						{
							// if SL order is present at Queue.front and volume rule is on and multBar rule if OFF then cancel new order which just came
							(*orderMap).erase(Order.orderID);
							stOb->onOrderResponse('X',Order.orderID,Order.symbol,Order.side,Order.quantity,Order.price);   // cancel order success response
						}
						else
						{
							ordersQueue[symbl].push(Order);
							if(MatchingBar == 0)
								checkOrderFromQueue(symbl);
						}
					}
					else
						sendToMatch(Order,Cur);
				}
				else
				{
					#ifdef debugOM
					cout << "Order rejected in moidfying due to inapropriate values/params\n";
					printOrder(Order,symbl);
					#endif

					stOb->onOrderResponse('R',Order.orderID,Order.symbol,Order.side,Order.quantity,Order.price);			
				}
			}
		else if(Order.orderType == 'N' && checkValidOrder(Order,Cur) && !checkInternalOrderCross(Order))
		{
			(*orderMap)[Order.orderID] = Order;

			if(DataFreq == "SNAPSHOT")
			{
				vector< pair<long long int, long long int> > temporary;						// creating dummy if map[symbol] is empty
				if(Order.side == 'S' && symbolToSellOrdID.find(Order.symbol) != symbolToSellOrdID.end())		/*** calling find function in map twice here ***/
					temporary = symbolToSellOrdID[Order.symbol];
				if(Order.side == 'B' && symbolToBuyOrdID.find(Order.symbol) != symbolToBuyOrdID.end())
					temporary = symbolToBuyOrdID[Order.symbol];

				insertOrderInSymbolMap(temporary,Order);						// sending OrderIDs of current symbol and Order to insert into map
			}

			stOb->onOrderResponse('N',Order.orderID,Order.symbol,Order.side,Order.quantity,Order.price);
				
			if(DataFreq == "TBT")
				orderBOb->recOrderData(Order);
			else if(DataFreq == "MINUTELY")			// for next bar matching
			{
				// symbolToOrderID[symbl] = Order;
				auto it = ordersQueue.find(symbl);
				if(VolumeBasedMatching && !OrderMatchingInMultBars && !it->second.empty() && it->second.front().matchRemainingInNeXtBarOverRide)
				{
					// if SL order is present at Queue.front and volume rule is on and multBar rule if OFF then cancel new order which just came
					(*orderMap).erase(Order.orderID);
					stOb->onOrderResponse('X',Order.orderID,Order.symbol,Order.side,Order.quantity,Order.price);   // cancel order success response
				}
				else
				{
					ordersQueue[symbl].push(Order);
					if(MatchingBar == 0)
						checkOrderFromQueue(symbl);
				}
			}
			else
				sendToMatch(Order,Cur);
		}
		else
		{
			#ifdef debugOM
			cout << "Order rejected in third else condidtion in OM\n";
			printOrder(Order,symbl);
			#endif
			
			stOb->onOrderResponse('R',Order.orderID,Order.symbol,Order.side,Order.quantity,Order.price);
		}
	}
}

bool orderManager::checkValidOrder(OrderSt Order,DataSt Cur)							// Check if order values are appopriate 
{
	if(Order.orderID<=0 || Order.orderID>6000000000000000000)
		return false;
	if(Order.side!='S' && Order.side!='B')
		return false;
	if(Order.quantity<=0 || Order.quantity>100000000 || Order.quantity%Cur.lot!=0)
		return false;
	if(Order.price<0 || Order.price>1000000000000 || Order.price%5!=0)
		return false;

	return true;

}

bool orderManager::checkInternalOrderCross(OrderSt Order)	// check if NEW order isin't matching with our previously created orders (which werent Traded)
{
	if(DataFreq == "SNAPSHOT")							
	{
		if(Order.side == 'B')
		{
			auto it = symbolToSellOrdID.find(Order.symbol);
			if(it!=symbolToSellOrdID.end() && (it->second)[0].second <= Order.price )		// check if New Buy Order price is greater than smallest sell price in map
			{
				#ifdef debugOM
				cout << "Inernal Sell Orders crossing : " <<  (it->second)[0].second << endl;
				#endif
				return true;													
			}
		}
		else
		{
			auto it = symbolToBuyOrdID.find(Order.symbol);
			if(it!=symbolToBuyOrdID.end() && (it->second).back().second >= Order.price )	// check if New Sell Order price is smaller than greatest buy price in map
			{
				#ifdef debugOM
				cout << "Inernal Buy Orders crossing : " <<  (it->second).back().second << endl;
				#endif
				return true;
			}
		}
	}

	return false;
}

char orderManager::sendToMatch(OrderSt Order, DataSt Cur)				// send to MM
{
	char tradeRes = matchingOb->matchOrders(Order,Cur,Trade);
	/*** tradeREs == 'P' means Trade is partial success and MM can(may be) match more trades with same OrderBook 
	 * 		tradesRes == 'p' means Trade is partial and rest of the order is Cancelled/Rejected ***/
	if(tradeRes == 'T' || tradeRes == 'P' || tradeRes == 'p')													// if trade went through
	{
		if((*orderMap)[Trade.orderID].quantity == Trade.quantity)
		{
			if(DataFreq == "SNAPSHOT")										// only for SNAPSHOT
				deleteOrderInSymbolMap((*orderMap)[Trade.orderID]);			// deleteing OrderID from symbol map too
			(*orderMap).erase(Trade.orderID);
		}
		else if((*orderMap)[Trade.orderID].quantity > Trade.quantity)
			(*orderMap)[Trade.orderID].quantity = (*orderMap)[Trade.orderID].quantity - Trade.quantity;

		if(Trade.quantity > 0)
		{
			tradesDOb->recTradeDetails(Trade);				// sending Trade info for analysis
			// tradeDump.push_back(tradeInfo);
			// pushing trade information to this tradeDump vector 
			
			stOb->onOrderResponse(tradeRes,Trade.orderID,Trade.symbol,Trade.side,Trade.quantity,Trade.price);		// sedning Trade Response to Strat
		}

		if(tradeRes == 'p')
		{
			(*orderMap).erase(Order.orderID);		// erasing cancelled order
			stOb->onOrderResponse('X',Order.orderID,Order.symbol,Order.side,Order.quantity-Trade.quantity,Order.price);		// sedning Cancel Response to Strat
		}

		if(tradeRes == 'P' && DataFreq == "SNAPSHOT")					//	check if more Trade is possible with same OrderID
		{
			Order.quantity = Order.quantity - Trade.quantity;
			sendToMatch(Order,Cur);
		}

		return tradeRes;
	}

	return tradeRes;
}

void orderManager::orderBookResponseTBT(std::string symbol, char s, long long px, int qt, char mType, long long time,long long OID1,long long OID2)
{
	if(mType == 'X')
	{
		(*orderMap).erase(OID1);		
		stOb->onOrderResponse(mType,OID1,symbol,s,qt,px);
	}
	else
	{
		if(OID1 >= OrderIDStartingNumber)
		{
			if((*orderMap)[OID1].quantity == qt)
			{
				(*orderMap).erase(OID1);
			}
			else if((*orderMap)[OID1].quantity > qt)
				(*orderMap)[OID1].quantity = (*orderMap)[OID1].quantity - qt;

		    Trade.symbol = symbol;
		    Trade.side = s;
		    Trade.price = px;
		    Trade.quantity = qt;
			tradesDOb->recTradeDetails(Trade);				// sending Trade info for analysis
			// tradeDump.push_back(tradeInfo);
			// pushing trade information to this tradeDump vector 
			#ifdef debugOM
			cout << "trade.time" << time << '\n';
			#endif
			stOb->onOrderResponse(mType,OID1,symbol,s,qt,px);	
		}
		if(OID2 >= OrderIDStartingNumber)
		{
			if((*orderMap)[OID2].quantity == qt)
			{
				(*orderMap).erase(OID2);
			}
			else if((*orderMap)[OID2].quantity > qt)
				(*orderMap)[OID2].quantity = (*orderMap)[OID2].quantity - qt;
	
			Trade.symbol = symbol;
		    Trade.side = s;
		    Trade.price = px;
		    Trade.quantity = qt;
			tradesDOb->recTradeDetails(Trade);				// sending trade info for analysis
			// tradeDump.push_back(tradeInfo);
			// pushing trade information to this tradeDump vector 
			#ifdef debugOM
			cout << "trade.time " << time << '\n';
			#endif
			stOb->onOrderResponse(mType,OID2,symbol,s,qt,px);	
		}
	}
}

void orderManager::giveTradeOfSquareOffOrders()
{
	for(auto ordersQueueIter = ordersQueue.begin();ordersQueueIter != ordersQueue.end();ordersQueueIter++)
		while(!ordersQueueIter->second.empty() && (sendToMatch(ordersQueueIter->second.front(),(*dataMap)[ordersQueueIter->second.front().symbol])) == 'T')
	    	ordersQueueIter->second.pop();
}

// bool orderManager::dumpTradeInfoToFile(string path)	
// {

// 	// ofstream ofs (path, ofstream::out);			// output file stream
// 	// for(int i=0;i<tradeDump.size();i++)
// 	// {
// 	// 	// if(tradeDump[i][8] == "B")
// 	// 	// {
// 	// 	// 	buyDump += stoi(tradeDump[i][6])*stoi(tradeDump[i][7]);
// 	// 	// 	interestQuant += stoi(tradeDump[i][7]);
// 	// 	// }
// 	// 	// if(tradeDump[i][8] == "S")
// 	// 	// {
// 	// 	// 	sellDump += stoi(tradeDump[i][6])*stoi(tradeDump[i][7]);
// 	// 	// 	interestQuant -= stoi(tradeDump[i][7]);
// 	// 	// }

// 	// 	for(int j=0;j<tradeDump[i].size();j++)
// 	// 		ofs << tradeDump[i][j] << " ";			// push in output file
// 	// 	ofs << "\n";
// 	// }
// 	// ofs << buyDump << " " << sellDump << " " << interestQuant << "\n";
// 	// ofs.close();

// 	return true;
// }

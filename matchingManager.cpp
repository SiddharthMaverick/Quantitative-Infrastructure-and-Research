#include "matchingManager.h"

matchingManager::matchingManager(unordered_map<string,DataSt> &dataM)
{
	dataMap = &dataM;
}	

char matchingManager::matchOrders(OrderSt Order, DataSt Cur, TradeMsgSt &Trade)
{
	if(Cur.time > LTT)		// if we moved to nextBar
	{
		LTT = Cur.time;		// update the time when last Time matching was called
		quantityTradeInCurBar = 0;	// making quantity Trade in current bar to be 0
	}

	switch (MatchingRuleNumber)
	{
		case 1:
		{
			if(DataFreq == "SNAPSHOT")
			{
				vector<std::pair<int,int> > :: iterator it;								// Original Order Book's iterator
				if(Order.side == 'B' && (it = Cur.sellDepth.begin())->first <= Order.price)		// buy Order
				{
					if(Order.quantity <= it->second)									// if quantity of Order can be Traded fully
					{
						fillTradeDetails(Trade,Order.orderID,Order.symbol,Order.side,Order.quantity,it->first,Cur.time,Cur.date,Cur.expiry,Cur.lot,Cur.volume,Order.matchRemainingInNeXtBarOverRide);
						it->second = it->second - Order.quantity;
						if(it->second == 0)
						{
							Cur.sellDepth.erase(it);	
							(*dataMap)[Order.symbol] = Cur;
						}
					}
					else																		// ORder.quantity > it->second
					{
						fillTradeDetails(Trade,Order.orderID,Order.symbol,Order.side,it->second,it->first,Cur.time,Cur.date,Cur.expiry,Cur.lot,Cur.volume,Order.matchRemainingInNeXtBarOverRide);
						Cur.sellDepth.erase(it);
						(*dataMap)[Order.symbol] = Cur;
						return 'P';																// partial Trade and can Trade more
					}

					return 'T';
				}
				else if(Order.side == 'S' && (it = Cur.buyDepth.begin())->first >= Order.price)	// sell Order
				{
					if(Order.quantity <= it->second)
					{
						fillTradeDetails(Trade,Order.orderID,Order.symbol,Order.side,Order.quantity,it->first,Cur.time,Cur.date,Cur.expiry,Cur.lot,Cur.volume,Order.matchRemainingInNeXtBarOverRide);
						it->second = it->second - Order.quantity;
						if(it->second == 0)
						{
							Cur.buyDepth.erase(it);	
							(*dataMap)[Order.symbol] = Cur;
						}
					}
					else																		// ORder.quantity > it->second
					{
						fillTradeDetails(Trade,Order.orderID,Order.symbol,Order.side,it->second,it->first,Cur.time,Cur.date,Cur.expiry,Cur.lot,Cur.volume,Order.matchRemainingInNeXtBarOverRide);
						Cur.buyDepth.erase(it);
						(*dataMap)[Order.symbol] = Cur;
						return 'P';																// partial Trade can be Traded again
						// erase from book too
						/*** return 'P' as partial Trade and it can trade more
						 ***/
					}
					return 'T';
				}
			}

			if(DataFreq == "MINUTELY")
				if(Order.side == 'B')
				{
					return fillTradeDetails(Trade,Order.orderID,Order.symbol,Order.side,Order.quantity,Cur.open,Cur.time,Cur.date,Cur.expiry,Cur.lot,Cur.volume,Order.matchRemainingInNeXtBarOverRide);
					// return 'T';
				}
				else if(Order.side == 'S')
				{
					return fillTradeDetails(Trade,Order.orderID,Order.symbol,Order.side,Order.quantity,Cur.open,Cur.time,Cur.date,Cur.expiry,Cur.lot,Cur.volume,Order.matchRemainingInNeXtBarOverRide);
					// return 'T';
				}
		}
		break;

		case 2:
		{
			if(Order.side == 'B')
			{
				return fillTradeDetails(Trade,Order.orderID,Order.symbol,Order.side,Order.quantity,Cur.high,Cur.time,Cur.date,Cur.expiry,Cur.lot,Cur.volume,Order.matchRemainingInNeXtBarOverRide);
				// return 'T';
			}
			else if(Order.side == 'S')
			{
				return fillTradeDetails(Trade,Order.orderID,Order.symbol,Order.side,Order.quantity,Cur.low,Cur.time,Cur.date,Cur.expiry,Cur.lot,Cur.volume,Order.matchRemainingInNeXtBarOverRide);
				// return 'T';
			}
		}
		break;

		case 3:
		{
			if(Order.side == 'B')
			{
				return fillTradeDetails(Trade,Order.orderID,Order.symbol,Order.side,Order.quantity,Cur.close,Cur.time,Cur.date,Cur.expiry,Cur.lot,Cur.volume,Order.matchRemainingInNeXtBarOverRide);
				// return 'T';	
			}
			else if(Order.side == 'S')
			{
				return fillTradeDetails(Trade,Order.orderID,Order.symbol,Order.side,Order.quantity,Cur.close,Cur.time,Cur.date,Cur.expiry,Cur.lot,Cur.volume,Order.matchRemainingInNeXtBarOverRide);
				// return 'T';
			}
		}
		break;

		case 4:
		{
			if(Order.side == 'B')
			{
				return fillTradeDetails(Trade,Order.orderID,Order.symbol,Order.side,Order.quantity,(Cur.high+Cur.close)/2,Cur.time,Cur.date,Cur.expiry,Cur.lot,Cur.volume,Order.matchRemainingInNeXtBarOverRide);
				// return 'T';
			}
			else if(Order.side == 'S')
			{
				return fillTradeDetails(Trade,Order.orderID,Order.symbol,Order.side,Order.quantity,(Cur.low+Cur.close)/2,Cur.time,Cur.date,Cur.expiry,Cur.lot,Cur.volume,Order.matchRemainingInNeXtBarOverRide);
				// return 'T';
			}
		}
		break;

		default:
			return '0';
	}

	return '0';
}

char matchingManager::fillTradeDetails(TradeMsgSt &Trade, long long int orderID,string symbol,char side,int quantity,long long int price,long long int time,string date,string expiry,int lot,int volume, bool matchRemainingInNeXtBarOverRide)
{
	Trade.orderID = orderID;  // same order id which was sent through strategy
	Trade.symbol = symbol;
	Trade.side = side;
	Trade.time = time;
	Trade.date = date;
	Trade.expiry = expiry;
	Trade.lot = lot;
	Trade.msgType = 'T';
	#ifdef debugMM
	std::cout << time << '\n';
	#endif
	if(DataFreq == "MINUTELY")
	{
		if(side == 'B')
			Trade.price = price*(1+((double)Slippage)/10000);
		else if(side == 'S')
			Trade.price = price*(1-((double)Slippage)/10000);
	}
	else
		Trade.price = price;

	if(DataFreq == "MINUTELY" && VolumeBasedMatching)			// if volume based rule is on
	{
		int maxVol = volume*PercentageVolumeMatching/100;
		if(DataType == "FO")
			maxVol = (maxVol/lot)*lot;
		#ifdef debugMM
		std::cout << maxVol << ' ' << volume << ' ' << quantity << ' ' << quantityTradeInCurBar << '\n'; 	
		#endif
		if(quantity + quantityTradeInCurBar <= maxVol)	// if full quantity can be traded
		{
			Trade.quantity = quantity;
			quantityTradeInCurBar += Trade.quantity;
			return 'T';
		}
		else if(quantityTradeInCurBar < maxVol)	// if some quantity can be traded in CurBar
		{
			Trade.quantity = maxVol - quantityTradeInCurBar;
			quantityTradeInCurBar += Trade.quantity;
			if(OrderMatchingInMultBars || matchRemainingInNeXtBarOverRide)	// for SL orders mult bar matching is overwritten by True
				return 'P';
			return 'p';
		}
		else if(!OrderMatchingInMultBars && !matchRemainingInNeXtBarOverRide) // for SL orders mult bar matching is overwritten by True
		{
			Trade.quantity = 0;
			return 'p';
			// add cases if quantity that can be Trade is 0, and next bar matching is OFF return 'p'
		}
		return '0';
	}
	else 							// if volume based rule is off
	{
		Trade.quantity = quantity;
		return 'T';
	}

	#ifdef debugMM
	cout << Trade.symbol << " is " << Trade.side << " " << Trade.quantity << " quant for " << Trade.price << " at time " << Trade.time << endl;
	#endif
}

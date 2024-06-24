#include "tradesDump.h"

tradesDump::tradesDump(unordered_map<string,DataSt> &dataM)
{
	dataMap = &dataM;
}

void tradesDump::doAction(long long curTime)
{
	int curHHMMSS = getCurrentMinute(curTime);					// returns current HHMMSS
	if(checkMinuteIncrement(curHHMMSS))
	{
		calcPNL();
		calcExposure(curHHMMSS);
	}
}

void tradesDump::calcExposure(long long cTime)
{
	cTime /= 100; 						// convert cTime from HHMMSS to HHMM
	cTime = cTime%100 + (cTime/100)*60;	// convert cTime from HHMM to number of total minutes i.e. HH * 60 + MM
	if(cTime > lastExposureTime + 10)	// if last time when exposure was calculated was 10 minutes ago
	{
		lastExposureTime = cTime;
		long long curExposure = 0;
		for(auto it = totalTradesData.begin();it!=totalTradesData.end();it++)	// for all symbols
		{
			if(it->second.buyQuant != it->second.sellQuant)
				curExposure += abs(it->second.buyQuant - it->second.sellQuant) * lastTrade[it->first].price; // ltp(our last Trade) * netQuant
	//		std::cout << it->first << ' ' << it->second.buyQuant - it->second.sellQuant << ' ' << lastTrade[it->first].price << ' ';
		}
		//std::cout << curExposure << '\n';
		exposureArray.push_back(curExposure);
	}
}

int tradesDump::getCurrentMinute(long long ctime)
{
	if(DataFreq == "MINUTELY")
	{
		// vector<string> v; 						// contains comma seperated strings
	 //    stringstream ss(dataString);
 	
  //   	while (ss.good()) 
  //   	{
	 //        string substr;
	 //        getline(ss, substr, ',');
	 //        v.push_back(substr);
	 //    }
 
		// return stoi(v[2].substr(0,2))*100 + stoi(v[2].substr(3,5));				// HH*100 + MM

		return int(ctime);
	}

	if(DataFreq == "SNAPSHOT")
	{
		// vector<string> v; 						// contains comma seperated strings
	 //    stringstream ss(dataString);
	 	
	 //    while (ss.good()) 
	 //    {
	 //        string substr;
	 //        getline(ss, substr, '^');
	 //        v.push_back(substr);
	 //    }

		time_t epochTime = 315513000 + (int)ctime;		// 315513000 added in 1980year time to make it 1970 time 
		tm *timeStructure = localtime(&epochTime);
		return timeStructure->tm_hour*100+timeStructure->tm_min;				// HH*100 + MM
	}

	if(DataFreq == "TBT")
	{
		// index = 0;
	 //    stringstream ss(dataString);
	 	
	 //    while (ss.good()) {
	 //        getline(ss, substrTemp, ',');
	 //        vTemp[index++] = substrTemp;
	 //    }

	 //    if(index!=10)		// if input dataSeperated Strings are size 10, then its valid line to read
	 //    	return false;		// corrupt line

	    time_t epochTime = 315513000 + (int)(ctime/1000000000);		// 315513000 added in 1980year time to make it 1970 time 
		tm *timeStructure = localtime(&epochTime);
		return timeStructure->tm_hour*100+timeStructure->tm_min;				// HH*100 + MM
	}

	return 0;
}

bool tradesDump::checkMinuteIncrement(int HHMM)
{
	if(curMinute < HHMM)					// time in hour and minutes e.g. 9:36 as 936
	{
		curMinute = HHMM;
		return true;
	}
	return false;
}

float tradesDump::getTransactionCost(string sym,long long price,int quantity,char side)
{
	if(DataType == "EQ")
		return (side == 'B') ? (quantity*4)*((float)price/100000) : (quantity*29)*((float)price/100000);
	else if(DataType == "FO")
	{
		if(DataFreq == "SNAPSHOT" || (DataFreq == "MINUTELY" && IsOptions) || DataFreq == "TBT")
		{
			if(DataFreq == "MINUTELY")
				sym = sym.substr(0,sym.length()-4);											// remove ".NFO" from symbol end in case of minutely

			if(sym.back() == 'T')
				return (side == 'B') ? (quantity*25)*((float)price/1000000) : (quantity*125)*((float)price/1000000);
			else if (sym.back() == 'E')
				return (side == 'B') ? (quantity*56)*((float)price/100000) : (quantity*106)*((float)price/100000);
		}
		else if(DataFreq == "MINUTELY" && !IsOptions)										// DataType is FO and Options is false, means Futures only
			return (side == 'B') ? (quantity*25)*((float)price/1000000) : (quantity*125)*((float)price/1000000);

	}

	return 0;
}

void tradesDump::recTradeDetails(TradeMsgSt Trade)					// called from OM when Trade Occurs
{
	// vector<string> tradeInfo;
	// tradeInfo.push_back("Trade");tradeInfo.push_back(Date);tradeInfo.push_back(to_string(Trade.time));tradeInfo.push_back(Trade.symbol);tradeInfo.push_back(Trade.expiry);tradeInfo.push_back(to_string(Trade.lot));
	// tradeInfo.push_back(to_string(Trade.price));tradeInfo.push_back(to_string(Trade.quantity));tradeInfo.push_back(string(1,Trade.side));tradeInfo.push_back(to_string(Trade.orderID));
	// tradeDump.push_back(tradeInfo);

	lastTrade[Trade.symbol] = Trade;			// adding current trade to LastTradeMap of that symbol

	auto it = totalTradesData.find(Trade.symbol);

	TradesTotalSt TTSt;
	if(it == totalTradesData.end())									// initializing if first Trade occurs for a symbol
	{
		TTSt.buyVal=0;TTSt.sellVal=0;TTSt.buyQuant=0;TTSt.sellQuant=0;TTSt.buyTrades=0;TTSt.sellTrades=0;TTSt.buyTransactionCost=0;TTSt.sellTransactionCost=0;
	}
	else															// else get Trade info till now
		TTSt = it->second;											

	if(Trade.side == 'B')											// append new Trade information
	{
		TTSt.buyVal += Trade.price*Trade.quantity;
		TTSt.buyQuant += Trade.quantity;
		TTSt.buyTrades++;
		TTSt.buyTransactionCost += getTransactionCost(Trade.symbol,Trade.price,Trade.quantity,Trade.side);
		#ifdef debugTradeDump
		cout << Trade.symbol << " B " << Trade.price << ' ' << Trade.quantity << ' ' << TTSt.buyTrades << ' ' << TTSt.buyTransactionCost << '\n';
		#endif
	}
	else if(Trade.side == 'S')										// append new Trade information
	{
		TTSt.sellVal += Trade.price*Trade.quantity;
		TTSt.sellQuant += Trade.quantity;
		TTSt.sellTrades++;
		TTSt.sellTransactionCost += getTransactionCost(Trade.symbol,Trade.price,Trade.quantity,Trade.side);
		#ifdef debugTradeDump
		cout << Trade.symbol << " S " << Trade.price << ' ' << Trade.quantity << ' ' << TTSt.sellTrades << ' ' << TTSt.sellTransactionCost << '\n';
		#endif
	}
		
	totalTradesData[Trade.symbol] = TTSt;							// put back into the Map after updating 

}

void tradesDump::calcPNL()											// cal PNL after certain amount of fixed time
{
	#ifdef debugTradeDump
	cout << curMinute << "\n";
	#endif

	TradesTotalSt TTSt;
	long long int LastTradesPrice;											// LTP would mean different for differet Data e.g. for minutely LTP = close

	for(auto it = totalTradesData.begin();it!=totalTradesData.end();it++)
	{
		TTSt = it->second;
		if(DataFreq == "MINUTELY")
			LastTradesPrice = (*dataMap)[it->first].close;
		else if(DataFreq == "SNAPSHOT" || DataFreq == "TBT")
			LastTradesPrice = (*dataMap)[it->first].LTP;
		
		
		int netQuantity = TTSt.buyQuant - TTSt.sellQuant;
		float transactionCostOnLTP = getTransactionCost(it->first,LastTradesPrice,(netQuantity>0)?netQuantity:-netQuantity,(netQuantity)>0?'S':'B');
		pnl[it->first].push_back(TTSt.sellVal - TTSt.buyVal + (netQuantity * LastTradesPrice) - (int)(TTSt.buyTransactionCost + TTSt.sellTransactionCost + transactionCostOnLTP));		// push current LTP in vector
		
		#ifdef debugTradeDump
		cout << std::fixed << std::setprecision(2) << it->first << " pnl:" << (float(TTSt.sellVal - TTSt.buyVal + (netQuantity * LastTradesPrice) - (int)TTSt.buyTransactionCost - (int)TTSt.sellTransactionCost - (int)transactionCostOnLTP))/100
		<< " selVal:" << ((float)TTSt.sellVal)/100 << " buyVal" << ((float)TTSt.buyVal)/100 << " netBoughtQuant:" << (TTSt.buyQuant - TTSt.sellQuant) << " LTP:" << ((float)LastTradesPrice)/100 << endl;
		#endif
	}

}

long double tradesDump::calcMedian(vector<long long int> scores)
{
  size_t size = scores.size();

	if (scores.size() == 0)
		return 0;  // empty.
	else
	{
		sort(scores.begin(), scores.end());
		if (size % 2 == 0)
			return (long double)(scores[size / 2 - 1] + scores[size / 2]) / 200;			// converting to Rs from paisa
		else 
			return (long double)scores[size / 2] / 100;									// converting to Rs from paisa
	}
}

long long calcDrawdown(vector<long long> pnl)
{
	if(pnl.size()==0)
		return 0;

	// long long maxPNL = numeric_limits<long long int>::min();
	long long int drawdown = 0;
	for(int i=0;i<pnl.size();i++)
	{
		// maxPNL = max(maxPNL,pnl[i]);
		for(int j=i+1;j<pnl.size();j++)
			drawdown = max(drawdown,pnl[i] - pnl[j]);
	//	cout << i << ' ' << drawdown << " drawdown\n";
	}

	return drawdown;
	// vector<long long> cumPNL(pnl.size(),0);
	// if(cumPNL.size()==0)
	// 	return 0;

	// cumPNL[0] = pnl[0];
	// for(int i=1;i<cumPNL.size();i++)
	// 	cumPNL[i] = cumPNL[i-1] + pnl[i];

	// long long maxPNL = numeric_limits<long long int>::min();
	// long long int drawdown = 0;
	// for(int i=0;i<cumPNL.size();i++)
	// {
	// 	maxPNL = max(maxPNL,cumPNL[i]);
	// 	for(int j=i+1;j<cumPNL.size();j++)
	// 		drawdown = max(drawdown,maxPNL - cumPNL[j]);
	// }

	// return drawdown;
}

std::string tradesDump::getExposureData()
{
	sort(exposureArray.begin(),exposureArray.end());
	std::cout << exposureArray.size() << ' ' << exposureArray.size()*.75 << ' ' << exposureArray.size()*.50 << '\n';
	return " exposureMax:" + std::to_string(exposureArray.back()/100) + " exposure75:" + std::to_string(exposureArray[exposureArray.size()*.75]/100) +
	" exposure50:" + std::to_string(exposureArray[exposureArray.size()*.50]/100) + " exposureAvg:" + std::to_string(accumulate(exposureArray.begin(), exposureArray.end(), 0.0)/100/exposureArray.size());
}

void tradesDump::dumpTradeInfoToFile()	
{
	calcPNL();												// calculating PNL in Last Minute too, is case of any Trades happened in LAst Minute

	tradeDumpFile = "./summary/" + DataFreq + "_" + DataType + "_" + Date + ".txt"; 
	//tradeDumpFile = "summary/" + DataFreq + "_" + DataType + "_" + Date + ".txt";		// dump file name
	ofstream ofs (tradeDumpFile, ofstream::out);			// output file stream

	vector<long long> medianMinPNL,medianMaxPNL,medianDrawdown,medianFinalPNL,medianBuyValue,medianSellValue;
	long long totalBuyTransactionCost,totalSellTransactionCost,totalFinalPNL;
	int totalBuyTrades,totalSellTrades;
	totalBuyTransactionCost = totalSellTransactionCost = totalFinalPNL=0;
	totalBuyTrades=totalSellTrades=0;

	for(auto it = pnl.begin();it!=pnl.end();it++)
	{
		string symbol = it->first;
		ofs << symbol;
		vector<long long> pnlVector = it->second;

		long long int maxPNL,minPNL,finalPNL,drawdown=0;
		minPNL = numeric_limits<long long int>::max();
		maxPNL = numeric_limits<long long int>::min();
		for(int i=0;i<pnlVector.size();i++)
		{
			minPNL = min(minPNL,pnlVector[i]);				// calCulate mininmum and maximum PNL
			maxPNL = max(maxPNL,pnlVector[i]);
		}

		drawdown = calcDrawdown(pnlVector);
		finalPNL = pnlVector.back();						// last element of PNLvector
		TradesTotalSt T = totalTradesData[symbol];
		ofs << std::fixed << std::setprecision(2) << " minPNL:" << ((float)minPNL)/100 << " maxPNL:" << ((float)maxPNL)/100 << " Drawdown:" << ((float)drawdown)/100  << " FinalPNL:" << ((float)finalPNL)/100
		<< " MedianPNL:"<< calcMedian(pnlVector) << " buyVal:" << T.buyVal/100 << " sellVal:" << (T.sellVal)/100 << " buyQuant:" << T.buyQuant << " sellQuant:" << T.sellQuant << " buyTrades:"
		 << T.buyTrades << " sellTrades:" << T.sellTrades << " buyTransactionCost:" << ((float)T.buyTransactionCost)/100 << " sellTransactionCost:"
		 << ((float)T.sellTransactionCost)/100 << '\n';

		medianMinPNL.push_back(minPNL);medianMaxPNL.push_back(maxPNL);medianDrawdown.push_back(drawdown);medianFinalPNL.push_back(finalPNL);medianBuyValue.push_back(T.buyVal);
		medianSellValue.push_back(T.sellVal);
		totalBuyTransactionCost += ((long long)T.buyTransactionCost);
		totalSellTransactionCost += ((long long)T.sellTransactionCost);
		totalFinalPNL += finalPNL;
		totalBuyTrades += T.buyTrades;
		totalSellTrades += T.sellTrades;
	}
	
	ofs << std::fixed << std::setprecision(2) << "Total" << " medianMinPNL:" << calcMedian(medianMinPNL) << " medianMaxPNL:" << calcMedian(medianMaxPNL) << " medianDrawdown:" << calcMedian(medianDrawdown) 
	<< " medianFinalPNL:" << calcMedian(medianFinalPNL) << " totalFinalPNL:"<< totalFinalPNL/100 << " medianBuyValue:" << calcMedian(medianBuyValue) << " medianSellValue:"
	<< calcMedian(medianSellValue) << " totalBuyTransactionCost:" << totalBuyTransactionCost/100 << " totalSellTransactionCost:" << totalSellTransactionCost/100 << " totalBuyTrades:" << totalBuyTrades << " totalSellTrades:" << totalSellTrades << getExposureData() << "\n";

	// for(int i=0;i<tradeDump.size();i++)
	// {
	// 	// if(tradeDump[i][8] == "B")
	// 	// {
	// 	// 	buyDump += stoi(tradeDump[i][6])*stoi(tradeDump[i][7]);
	// 	// 	interestQuant += stoi(tradeDump[i][7]);
	// 	// }
	// 	// if(tradeDump[i][8] == "S")
	// 	// {
	// 	// 	sellDump += stoi(tradeDump[i][6])*stoi(tradeDump[i][7]);
	// 	// 	interestQuant -= stoi(tradeDump[i][7]);
	// 	// }

	// 	for(int j=0;j<tradeDump[i].size();j++)
	// 		ofs << tradeDump[i][j] << " ";			// push in output file
	// 	ofs << "\n";
	// }
	// ofs << buyDump << " " << sellDump << " " << interestQuant << "\n";
	ofs.close();
}


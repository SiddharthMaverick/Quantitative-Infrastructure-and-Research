#include "dataManager.h"

dataManager::dataManager(unordered_map<string,DataSt> &dataTemp, string &tempSymbol, string expiryPath, string lotSizePath)
{
        data = &dataTemp;                                                       // dataMap for Symbols
        currentSymbol = &tempSymbol;                            // Current Symbol read from datafile

		std::cout<<DataFilePath<<std::endl;
        if(DataFreq == "TBT" && DataType == "FO")
                loadStreamsFODummy(DataFilePath);
        else
        {
                myFile = new ifstream(DataFilePath);
                loadDataFile(myFile);
        }

        if(DataType == "FO")
        {
                loadExpiryFile(expiryPath);
                loadLotSizeFile(lotSizePath);
        }
        stringstream ss(ReqSymbols);                            //      reading valid Symbols from config ( these will be in mode 2 form)
                                                                                                 
        while (ss.good()) 
        {
                string substr;
                getline(ss, substr, ',');
                if(DataFreq == "MINUTELY" && DataType == "FO" && ModeWanted==1 && substr[substr.length()-1]=='T')       // only for minutely and future symbol conversrion from mode 2 to mode 1
                        substr = convertDataMode2to1(substr,1);		// if function called by inputSymbol the pass string,1
                validSymbols.insert(substr);
        }

    if(DataFreq == "SNAPSHOT")
                loadContractFile(ContractFilePath);

    #ifdef debugDM
    cout << "symbol in config: ";
    for(auto it = validSymbols.begin(); it != validSymbols.end(); it++)
        cout << *it << " ";
    cout << endl;
    #endif

}

void dataManager::loadStreamsFODummy(string path)
{
	int numOfstreams = 8;
	curExchangeTime = vector<long long> (numOfstreams,0);
	ifstream* myFileTBTFO;

	for(int i = 0;i < numOfstreams; i++)
	{
		streamRead = i;
		myFileTBTFO= new ifstream(path.substr(0,path.length()-4)+"_stream"+to_string(streamRead+1)+".txt");
		if(!myFileTBTFO->is_open())
		cout << path.substr(0,path.length()-4)+"_stream"+to_string(streamRead+1)+".txt" << " failed" << '\n';
		fileRead.push_back(myFileTBTFO);
	}
/*	streamRead = 0;
	myFileTest1= new ifstream("./dataFiles/TBT/2021/0409/Combined_FO_stream"+to_string(streamRead+1)+".txt");
	if(!myFileTest1->is_open())
	cout << "./dataFiles/TBT/2021/0409/Combined_FO_stream"+to_string(streamRead+1)+".txt" << " failed" << '\n';
	fileRead.push_back(myFileTest1);
	
	streamRead = 1;
	myFileTest1= new ifstream("./dataFiles/TBT/2021/0409/Combined_FO_stream"+to_string(streamRead+1)+".txt");
	if(!myFileTest1->is_open())
	cout << "./dataFiles/TBT/2021/0409/Combined_FO_stream"+to_string(streamRead+1)+".txt" << " failed" << '\n';
	fileRead.push_back(myFileTest1);
	
	streamRead = 2;
	myFileTest1= new ifstream("./dataFiles/TBT/2021/0409/Combined_FO_stream"+to_string(streamRead+1)+".txt");
	if(!myFileTest1->is_open())
	cout << "./dataFiles/TBT/2021/0409/Combined_FO_stream"+to_string(streamRead+1)+".txt" << " failed" << '\n';
	fileRead.push_back(myFileTest1);

	streamRead = 3;
	myFileTest1= new ifstream("./dataFiles/TBT/2021/0409/Combined_FO_stream"+to_string(streamRead+1)+".txt");
	if(!myFileTest1->is_open())
	cout << "./dataFiles/TBT/2021/0409/Combined_FO_stream"+to_string(streamRead+1)+".txt" << " failed" << '\n';
	fileRead.push_back(myFileTest1);

	streamRead = 4;
	myFileTest1= new ifstream("./dataFiles/TBT/2021/0409/Combined_FO_stream"+to_string(streamRead+1)+".txt");
	if(!myFileTest1->is_open())
	cout << "./dataFiles/TBT/2021/0409/Combined_FO_stream"+to_string(streamRead+1)+".txt" << " failed" << '\n';
	fileRead.push_back(myFileTest1);
	
	streamRead = 5;
	myFileTest1= new ifstream("./dataFiles/TBT/2021/0409/Combined_FO_stream"+to_string(streamRead+1)+".txt");
	if(!myFileTest1->is_open())
	cout << "./dataFiles/TBT/2021/0409/Combined_FO_stream"+to_string(streamRead+1)+".txt" << " failed" << '\n';
	fileRead.push_back(myFileTest1);
	
	streamRead = 6;
	myFileTest1= new ifstream("./dataFiles/TBT/2021/0409/Combined_FO_stream"+to_string(streamRead+1)+".txt");
	if(!myFileTest1->is_open())
	cout << "./dataFiles/TBT/2021/0409/Combined_FO_stream"+to_string(streamRead+1)+".txt" << " failed" << '\n';
	fileRead.push_back(myFileTest1);
  
	streamRead = 7;
	myFileTest1= new ifstream("./dataFiles/TBT/2021/0409/Combined_FO_stream"+to_string(streamRead+1)+".txt");
	if(!myFileTest1->is_open())
	cout << "./dataFiles/TBT/2021/0409/Combined_FO_stream"+to_string(streamRead+1)+".txt" << " failed" << '\n';
	fileRead.push_back(myFileTest1);
*/
/*	getline(*myFileTest1,tempString);
	decodeTBTDummy(tempString,curExchangeTime[streamRead]);
	(*data)[streamSymbols[streamRead]] = dataStreams[streamRead];
	*currentSymbol = streamSymbols[streamRead];
*/
//	./dataFiles/TBT/2021/0409/Combined_FO.txt	
}

bool dataManager::loadDataFile(ifstream *mFile)
{
    if(!mFile->is_open())
    {
    	cout << "error opening data file\n";
    	return false;
    }
    return true;
}

void dataManager::loadExpiryFile(string path)
{
	ifstream myFile(path);
	if(!myFile.is_open())
    	cout << "error opening expiry file\n";

    vector<string> expiryVector;				// temporary

    while(!myFile.eof())
    {
    	string tempString;
    	getline(myFile,tempString);
    	expiryVector.push_back(tempString);
    }

    myFile.close();

    for(int i=0;i<expiryVector.size();i++)
		if(expiryVector[i]>=Date)
		{
			F1 = expiryVector[i];			// defining future values from expiryFile
			F2 = expiryVector[i+1];
			F3 = expiryVector[i+2];
			break;
		}

	#ifdef debugDM
	cout << "expiry " << F1 << " " << F2 << " " << F3 << "\n";
	#endif
}

void dataManager::loadLotSizeFile(string path)
{
	ifstream myFile(path);
	if(!myFile.is_open())
    	cout << "error opening lot file\n";

    while(!myFile.eof())
    {
    	string tempString;
    	getline(myFile,tempString);				// extracting line from file
		if(tempString.length()==0)
			continue;

		vector<string> v; 						// contains comma seperated strings
	    stringstream ss(tempString);
	 	
	    while (ss.good()) {
	        string substr;
	        getline(ss, substr, ',');
	        v.push_back(substr);
	    }
	    string lotexpiry = v[2].substr(6,4) + v[2].substr(3,2) + v[2].substr(0,2);		// converting dd/mm/yy to yyyymmdd
	    if(lotexpiry == F1 || lotexpiry == F2 || lotexpiry == F3)						// if lotexpdate matches with expiry dates from F1/F2/F3
			lotMap[v[0] + lotexpiry] = stoi(v[3]);										// then insert "symbol(token) + expiry" as key and lot as value 
    }

    myFile.close();

    #ifdef debugDM
    cout << "lotMap :"<<lotMap.size() << endl;
    for(auto it = lotMap.begin();it != lotMap.end(); it++)
    	cout << it->first << " " << it->second << endl;
    #endif
}

void dataManager::loadContractFile(string path)
{
	ifstream myFile(path);
	if(!myFile.is_open())
    	cout << "error opening contract file\n";

    while(!myFile.eof())
    {
    	string tempString;
    	getline(myFile,tempString);				// extracting line from file
		if(tempString.length()==0)
			continue;

		vector<string> v; 						// contains comma seperated strings
	    stringstream ss(tempString);
	 	
	    while (ss.good()) {
	        string substr;
	        getline(ss, substr, '|');
	        v.push_back(substr);
	    }
	    
	    for(auto it = validSymbols.begin();it!=validSymbols.end();it++)
	    {
	    	string tempValidSymbol = (*it);
	    	if(DataType == "FO")
	    	{
	    		if(tempValidSymbol.back() == 'T' && v[8] == "XX" && v[53] == tempValidSymbol)		// 	SBIN20OCTFUT		match exact v[54] i.e 55th col
	    			tokenToSymbol[v[0]] = tempValidSymbol;
	    		else if(tempValidSymbol.back() == 'E' && (v[8] == "CE" || v[8] == "PE"))		//	SBIN28SEP20500PE
	    			if(v[3] + convertEpochToDate(v[6]) + v[7].substr(0,v[7].length()-2) + v[8] == tempValidSymbol)
	    				tokenToSymbol[v[0]] = tempValidSymbol;
	    	}
	    }
    }

    myFile.close();

	#ifdef debugDM
	cout << tokenToSymbol.size() << ":tokenMap: "<< endl;
	for(auto it = tokenToSymbol.begin();it!=tokenToSymbol.end();it++)
		cout << it->first << ":" << it->second << endl;
	#endif
}

// void dataManager::insertFixedValuesInMap()
// {
// 	if(DataType == "EQ")
// 	{
// 		DataSt tempDS;
// 		tempDS.lot = 1;
// 	    for(auto it = validSymbols.begin(); it != validSymbols.end(); it++)
// 	  		(*data)[*it] = tempDS;

// 	  	return;
// 	}

// 	if(DataFreq == "MINUTELY" && DataType == "FO" && !IsOptions && ModeWanted == 1)			// validSymbol will be ABC_F1 type 
// 	{
// 		DataSt tempDS;
// 	    for(auto it = validSymbols.begin(); it != validSymbols.end(); it++)
// 	  	{
// 	 		tempDS.expiry = getFutureExpiry(*it);			
// 	 		tempDS.lot = getLotSize(*it,tempDS.expiry);
// 	  		(*data)[*it] = tempDS;
// 	  	}

// 	  	return;
// 	}

//     for(auto it = validSymbols.begin(); it != validSymbols.end(); it++)
//   	{
//   		string tempSymbol = *it;
//   		if(DataFreq == "SNAPSHOT")
//   			tempSymbol = tempSymbol + ".NFO";								// to match both DataFreq input types

//   		DataSt tempDS;
// 		if(tempSymbol[tempSymbol.length()-5] == 'E')
//  		{
// 	 		tempDS.callput = tempSymbol.substr(tempSymbol.length()-6,2);
// 	 		int monthIndex = 0;												// index pointing towards last char of month e.g. N in JAN 
// 	 		for(monthIndex = tempSymbol.length()-7;monthIndex>3;monthIndex--)
// 	 			if(tempSymbol[monthIndex]>='A' && tempSymbol[monthIndex]<='Z')
// 	 				break;

// 	 		if(monthIndex>3)								// just to check if monthIndex has atleast 2 chataracters before it like JA before N in JAN
// 	 		{
// 				tempDS.expiry = "20" + tempSymbol.substr(monthIndex+1,2) + monthNumber[tempSymbol.substr(monthIndex-2,3)] + tempSymbol.substr(monthIndex-4,2);
// 				tempDS.strike = stoi(tempSymbol.substr(monthIndex+3,tempSymbol.length()-monthIndex-9));
// 				tempDS.lot = lotMap[tempSymbol.substr(0,monthIndex-4) + tempDS.expiry];			//	mathcing symbol + expiry in lotmap
// 	 		}											
// 	 	}
// 	 	else if(tempSymbol[tempSymbol.length()-5] == 'T')
// 	 	{
// 	 		tempDS.expiry = getFutureExpiry(tempSymbol); 		
// 	 		tempDS.lot = getLotSize(tempSymbol,tempDS.expiry);
// 	 	}

//   		(*data)[*it] = tempDS;
//   	}
	

// }

string dataManager::convertEpochToDate(string epoch)
{
	time_t epochTime = 315532800 + stoll(epoch);		// 315532800 added in 1980year time to make it 1970 time 
	tm *timeStructure = localtime(&epochTime);
	return to_string(timeStructure->tm_mday) + monthName[timeStructure->tm_mon + 1] + to_string((timeStructure->tm_year+1900)%100);
}

string dataManager::convertDataMode1to2(string v)						// data1 == bla_F1 ... data2 = bla19AUGFUT.NFO
{
	if(v.substr(max(0,(int)v.length()-3),v.length()) == "_F1")	// manipulating with strings
	{
		v = v.substr(0,(v).length()-3) + F1.substr(2,2) + monthName[stoi(F1.substr(4,2))] + "FUT.NFO";	// refer to .h for monthName
	}
	else if(v.substr(max(0,(int)v.length()-3),(v).length()) == "_F2")
	{
		v = v.substr(0,v.length()-3) + F2.substr(2,2) + monthName[stoi(F2.substr(4,2))] + "FUT.NFO";
	}
	else if((v).substr(max(0,(int)v.length()-3),v.length()) == "_F3")
	{
		v = v.substr(0,v.length()-3) + F3.substr(2,2) + monthName[stoi(F3.substr(4,2))] + "FUT.NFO";
	}

	return v;
}

string dataManager::convertDataMode2to1(string v, bool calledOnInputSymbols)
{
        if(v.length()<min_len_for_mode2_future_valid_symbol) {cout<<"Invalid Symbol for conversion :"<<v<<", exiting\n";exit(1);}
                                        										// X_20210101_FUT, minimum length should be 14
        stringstream ss(v);
        string symbol,futureNumber;
        if(ss.good()) getline(ss,symbol,'_');                                                                           // saving symbol from string stream into symbol 
        if(ss.good()) getline(ss,futureNumber,'_');                                                                     // saving futureNumber eg:(20210101) in futureNumber

        if(futureNumber == F1)          futureNumber = "F1";
        else if(futureNumber == F2)     futureNumber = "F2";
        else if(futureNumber == F3)     futureNumber = "F3";
	else if(calledOnInputSymbols)
	{cout<<"Invalid Expiry for Symbol :"<<v<<" ,exiting\n";exit(1);}

        return (symbol+"_"+futureNumber);										//returning the symbol 
}

string dataManager::getFutureExpiry(string v)
{
        if(v.substr(v.length()-3,2) == "_F")            							// dataMode1
        {
                if(v.back() == '1') return F1;
                if(v.back() == '2') return F2;
                if(v.back() == '3') return F3;
        }
        else if(v.length()>=min_len_for_mode2_future_valid_symbol)
        {                                                                                                       // data mode 2 ( X_20220101_FUT)
                stringstream ss(v);
                string symbol,futureNumber;
                if(ss.good()) getline(ss,symbol,'_');                                                           // X
                if(ss.good()) getline(ss,futureNumber,'_');                                                     // 20220101

                if(futureNumber == F1)          return F1;
                else if(futureNumber == F2)     return F2;
                else if(futureNumber == F3)     return F3;
        }
        return "";
}

int dataManager::getLotSize(string v, string expDate)
{
        if(v.substr(v.length()-3,2) == "_F")                                                                    // dataMode1
                return (lotMap.find(v.substr(0,v.length()-3) + expDate) != lotMap.end()) ? lotMap[v.substr(0,v.length()-3) + expDate] : 1;

        string symbol;stringstream ss(v);
        if(ss.good())
        {
                getline(ss,symbol,'_');
                return ( lotMap.find(symbol+expDate)!=lotMap.end() ) ?  lotMap[symbol+expDate]  : 1 ;
        }

        return 1;
}

uint8_t dataManager::fetchDataDummy(long long &cureTime)
{
	if(fileRead[streamRead]->eof())			// end of that Stream
	{
		curExchangeTime[streamRead]= numeric_limits<long long int>::max();		// if that stream end, put its exchange time to max
		for(int i=0;i<curExchangeTime.size();i++)
		{
			if(curExchangeTime[i] != numeric_limits<long long int>::max())		// check if there is atLeast 1 stream with exhcnageTime < max
				break;

			if(i == curExchangeTime.size()-1)	// condition for end of all StreamFiles
				return 0;						// return 0 means there is not more data
		}
	}
	else									// should read from that stream only if its not endOfFile
	{ 
		string tempString;
		getline(*fileRead[streamRead],tempString);
		usefulData[streamRead] = (decodeTBTDummy(tempString,curExchangeTime[streamRead]));
	} 

	minExchangeTime = curExchangeTime[0];
	streamRead = 0;
	for(int i=1;i<curExchangeTime.size();i++)
	{
		if(curExchangeTime[i]<minExchangeTime)
		{
			minExchangeTime = curExchangeTime[i];
			streamRead = i;
		}
	}
	if(usefulData[streamRead])
	{
		(*data)[streamSymbols[streamRead]] = dataStreams[streamRead];
		*currentSymbol = streamSymbols[streamRead];
		return 2;
	}

//	}
/*	cout << curExchangeTime[0] << ' ' << curExchangeTime[1] << '\n';	
	cout << tempString << '\n';
*/	
//	if(validSymbols.find(streamSymbols[streamRead])!=validSymbols.end())
//	{
//
//
//		string tempString;
//		getline(*fileRead[streamRead],tempString);
//		if(decodeTBTDummy(tempString,curExchangeTime[streamRead]))
//			return 2;
//	}


	return 1;
}

bool dataManager::decodeTBTDummy(string tempString,long long &time)
{
	index = 0;
    stringstream ss(tempString);
 	
    while (ss.good()) {
		getline(ss, substrTemp, ',');
        vTemp[index] = (substrTemp);
        index++;
    }

	if(index!=10)		// if input dataSeperated Strings are size 10, then its valid line to read
		return false;		// corrupt line

	time = stoll(vTemp[4]);

    string symbol;
    if(vTemp[3] == "N" || vTemp[3] == "M" || vTemp[3] == "X")	// ignoring ... || v[3] == "G" || v[3] == "H" || v[3] == "J"
    	symbol = vTemp[6];
    else if(vTemp[3] == "T")			// if message of TradeType and // ignoring ... || v[3] == "K"
    {
    	if(stoll(vTemp[5])==0 || stoll(vTemp[6])==0)	// if any of the orderId is 0 ,, it will be IOC order
    		symbol = vTemp[7];								
    	else
    		return false;									// // ignoring if tradeType and not IOC
    }

    if(validSymbols.find(symbol)!=validSymbols.end())
    {
    	DataSt tempDS;	
    	// tempDS.symbol = symbol;

    	if(vTemp[3] == "T")				// for IOC
		{		
	    	tempDS.messageType = 'I';		// putting  I for IOC order
	    	tempDS.orderID = 0;				// orderID will be 0 for IOC
	    	if(stoll(vTemp[5])==0)
				tempDS.side = 'B';				// converting 1 charater string to character
			else
				tempDS.side = 'S';
		} 
		else						// for M,M,X
		{
		   	tempDS.messageType = vTemp[3][0];		// N,M,X etc (first character of string)
	    	tempDS.orderID = stoll(vTemp[5]);
	    	tempDS.side = vTemp[7][0];				// converting 1 charater string to character
	    }

    	tempDS.time = stoll(vTemp[4]);
    	time = tempDS.time;

    	tempDS.price = stoll(vTemp[8]);
    	tempDS.quantity = stoi(vTemp[9]);
		tempDS.lot = 1;			/*** change this variable later later ***/	

		if(DataType == "FO")				// if FO data , we need to add extra fields
		{
			vector<string> symbolSeperated; 						// contains comma seperated strings
		    stringstream ss(symbol);
		 	
		    while (ss.good()) {
		        string substr;
		        getline(ss, substr, '_');
		        symbolSeperated.push_back(substr);
		    }

			if((symbol).back() == 'E')				// if options (not FUT)
	    	{
	    		tempDS.callput = symbolSeperated[3];
	    		tempDS.strike = stoi(symbolSeperated[2]);
			}

			//below statements are for both options and Futures
    		tempDS.expiry = symbolSeperated[1];
			string exp = convertEpochToDate(symbolSeperated[1]); // epoch ti expiry in DDMMMYY format
			
			string keyOfLotMap = symbolSeperated[0]+"20"+exp.substr(5,2)+monthNumber[exp.substr(2,3)]+exp.substr(0,2);
			//  convert DDMMMYY to YYYYMMDD abd then mathcing symbol + YYYYMMDD in lotmap 
			
			auto lotIter = lotMap.find(keyOfLotMap);

			if(lotIter != lotMap.end())
				tempDS.lot = lotIter->second;	
			else if(symbolSeperated[0] == "NIFTY" || symbolSeperated[0] == "BANKNIFTY" || symbolSeperated[0] == "FINNIFTY")
			{
				int dateTemp = stoi(keyOfLotMap.substr(symbolSeperated[0].length()));
				bool curMonthFlag = true;
				while(dateTemp%100<=31)
				{
					#ifdef debugDM
					cout << symbolSeperated[0]+to_string(dateTemp) << '\n';
					#endif
					if(lotMap.find(symbolSeperated[0]+to_string(dateTemp))!=lotMap.end())
					{	
						tempDS.lot = lotMap[keyOfLotMap] = lotMap[symbolSeperated[0]+to_string(dateTemp)];
						break;
					}
					dateTemp++;
					if(dateTemp%100==32 && curMonthFlag)
					{
						curMonthFlag = false;
						dateTemp = (dateTemp/100)*100 + 101;
					}
				}
			}
		}
    	
    	auto iter = (*data).find(symbol);
    	if(iter != (*data).end())
	    	tempDS.LTP = iter->second.LTP;
	    
		dataStreams[streamRead] = tempDS;

    	streamSymbols[streamRead] = symbol;

    	#ifdef debugDM
		cout << "\nReading TBT data in DM: " << symbol << " " << tempDS.messageType << " " << tempDS.time << " "
		<< tempDS.orderID << " " << tempDS.side << " " << tempDS.price << " " << tempDS.quantity << " exp"<<tempDS.expiry << 
		" strike"<< tempDS.strike << " l" << tempDS.lot <<"\n";
		#endif

    	return true;
    }
	return false;
}

uint8_t dataManager::fetchData(long long &curTime)										// return 0 if EOF, return 1 if garbage line, return 2 if read succes
{
	if(DataFreq == "TBT" && DataType == "FO")
		return fetchDataDummy(curTime);
	else if(!myFile->eof()) 
    {
    	string tempString;
		getline(*myFile,tempString);											// tempString contains full line of a file
		if(tempString.length()!=0)					
		{
			if(DataFreq == "MINUTELY")
				return (decodeMinutelyData(tempString,curTime)+1);						// 0+1 if Symbol's data is not required, 1+1 if req
			else if(DataFreq == "SNAPSHOT")
				return (decodeSnapshotData(tempString,curTime)+1);
			else if(DataFreq == "TBT")
				return (decodeTBTData(tempString,curTime)+1);
		}
		else 
			return 1;												// 1 if line is empty
    }
    myFile->close();
	return 0;
}

bool dataManager::decodeMinutelyData(string &tempString, long long &time)             					       // return true if useful data
{
    index = 0;
    stringstream ss(tempString);

    while (ss.good()) 
    {
                getline(ss, substrTemp, ',');
        vTemp[index] = (substrTemp);
        index++;
    }

        if(DataType=="FO" && ModeWanted==1 && vTemp[0][vTemp[0].length()-1]=='T')									
                vTemp[0]=convertDataMode2to1(vTemp[0],0);                                					 // converting symbol from mode 2 -> 1 after reading the input data file ... argument 0 is passed when this function is called in inputMinutely data
        if(validSymbols.find(vTemp[0])!=validSymbols.end())  					                           // matching valid Symbol name after converting and not converting data modes
                return(insertMinutelyDatatoDB(vTemp,time));

        return false;
}

bool dataManager::insertMinutelyDatatoDB(vector<string> v,long long &time)
{
        DataSt tempDS;

        tempDS.open = stoi(v[3]);
        if(tempDS.open == 0)   										                         // if we didnt have any trade in that bar(minute) , we will rec 0,0,0,0 OHLC
                return false;                                  									 // no ignore that data

        tempDS.date = v[1];                                                                                              
        tempDS.time = (stoi(v[2].substr(0,2)))*10000 + stoi(v[2].substr(3,5))*100;

        if(v[2].length()==8)
                tempDS.time += stoi(v[2].substr(6,8));

        time = tempDS.time;
        tempDS.high   = stoi(v[4]);
        tempDS.low    = stoi(v[5]);
        tempDS.close  = stoi(v[6]);
        tempDS.volume = atof(v[7].c_str());
        tempDS.OI     = atof(v[8].c_str());                                         
        tempDS.lot    = 1;

 	if(DataType == "FO")
        {
                if(v[0].substr(v[0].length()-1,1)=="E")                                                                         // ACC_20210101_2000_PE
                {
                        string KeyOfLotMap,symbol;
                        stringstream ss(v[0]);

                        if(ss.good()) getline(ss,symbol,'_');                                                                   // ACC
                        if(ss.good()) getline(ss,tempDS.expiry,'_');                                                            // 20210101
                        if(ss.good()) {string t;getline(ss,t,'_');tempDS.strike=stoi(t);}                                       // 2000
                        if(ss.good()) getline(ss,tempDS.callput,'_');                                                           // PE

                        string keyOfLotMap = symbol + tempDS.expiry;                                                            // ACC20210101
                        auto lotIter = lotMap.find(keyOfLotMap);
                        if(lotIter != lotMap.end()) tempDS.lot = lotIter->second;
                        else if(symbol == "NIFTY" || symbol == "BANKNIFTY" || symbol == "FINNIFTY")
                        {
                                int dateTemp = stoi(tempDS.expiry);
                                bool curMonthFlag = true;
                                while(dateTemp%100<=31)
                                {
                                        #ifdef debugDM
                                        cout << symbol +"_"+ to_string(dateTemp) <<" ";
                                        #endif
                                        if(lotMap.find(symbol+to_string(dateTemp)) != lotMap.end())
                                        {
                                                tempDS.lot = lotMap[keyOfLotMap] = lotMap[symbol+to_string(dateTemp)];
                                                break;
                                        }
                                        dateTemp++;
                                        if(dateTemp%100==32 && curMonthFlag)
                                        {
                                                curMonthFlag = false;
                                                dateTemp = (dateTemp/100)*100 + 101;
                                        }
                                }
                                #ifdef debugDM                  
                                        cout<<"\n";
                                #endif
                        }
                }
                else
                {
                        tempDS.expiry  = getFutureExpiry(v[0]);
                        tempDS.lot     = getLotSize(v[0],tempDS.expiry);
                        tempDS.callput = "NA";
                        tempDS.strike   = 0;
                }
	}
	
	(*data)[v[0]] = tempDS; 					                                                        // pushing in Datamap
        *currentSymbol = v[0];                                          					                // assigning current Symbol

        #ifdef debugDM
        cout << "Reading MINUTELY data in DM: " << *currentSymbol << " d" << tempDS.date << " t" << tempDS.time << " " << tempDS.open << " " <<
        tempDS.high << " " << tempDS.low << " " << tempDS.close << " v" << tempDS.volume << " oi" << tempDS.OI << " l" << tempDS.lot
        <<" e" << tempDS.expiry << " s" << tempDS.strike << " ce" << tempDS.callput << endl;
        #endif

        return true;
}

bool dataManager::decodeSnapshotData(string tempString,long long &time)
{
	// vector<string> v; 						// contains '^' seperated strings
 //    stringstream ss(tempString);
 	
 //    while (ss.good()) {
 //        string substr;
 //        getline(ss, substr, '^');
 //        v.push_back(substr);
 //    }

	index = 0;
    stringstream ss(tempString);
 	
    while (ss.good()) {
		getline(ss, substrTemp, '^');
        vTemp[index] = (substrTemp);
        index++;
    }

    if(vTemp[0]=="7202" && tokenToSymbol.find(vTemp[1])!=tokenToSymbol.end() && (*data).find(tokenToSymbol[vTemp[1]])!=(*data).end())
    	(*data)[tokenToSymbol[vTemp[1]]].OI = stoi(vTemp[2]);

    if(vTemp[0]!="7208")
    	return false;

    if(validSymbols.find(tokenToSymbol[vTemp[1]])!=validSymbols.end())	/*** calling tokenToSymbol Map twice ***/
    {
    	*currentSymbol = tokenToSymbol[vTemp[1]];					// assigning currentSymbol
    	DataSt tempDS;

    	tempDS.volume = atof(vTemp[2].c_str());
	    tempDS.LTP = stoi(vTemp[3]);
	    tempDS.LTQ = stoi(vTemp[5]);
	    tempDS.LTT = vTemp[6];

	    time = stoll(tempDS.LTT);

	    tempDS.ATP = stoi(vTemp[7]);
	    tempDS.close = stoi(vTemp[8]);
	    tempDS.open = stoi(vTemp[9]);
	    tempDS.high = stoi(vTemp[10]);
	    tempDS.low = stoi(vTemp[11]);
	    tempDS.lot = 1;
	    convertPipedDataToVector(tempDS.buyDepth,vTemp[12],vTemp[13]);
	    convertPipedDataToVector(tempDS.sellDepth,vTemp[14],vTemp[15]);

	    if(DataType == "FO")					// put lot and expiry in case of FUTURE and OPTIONS
	    {
	    	string symbol = *currentSymbol;			// copied to symbol to avoid multiple access to pointers
	    	if((symbol).back() == 'E')
	    	{
	    		tempDS.callput = symbol.substr(symbol.length()-2,2);
	    		int monthIndex = 0;												// index pointing towards last char of month e.g. N in JAN 
		 		for(monthIndex = symbol.length()-3;monthIndex>3;monthIndex--)
		 			if(vTemp[0][monthIndex]>='A' && vTemp[0][monthIndex]<='Z')
		 				break;

		 		if(monthIndex>3)								// just to check if monthIndex has atleast 2 chataracters before it like JA before N in JAN
		 		{
					tempDS.expiry = "20" + symbol.substr(monthIndex+1,2) + monthNumber[symbol.substr(monthIndex-2,3)] + symbol.substr(monthIndex-4,2);
					tempDS.strike = stoi(symbol.substr(monthIndex+3,symbol.length()-monthIndex-5));
//					tempDS.lot = lotMap[symbol.substr(0,monthIndex-4) + tempDS.expiry];			//	mathcing symbol + expiry in lotmap
				
					string keyOfLotMap = symbol.substr(0,monthIndex-4) + tempDS.expiry;
					//  convert DDMMMYY to YYYYMMDD abd then mathcing symbol + YYYYMMDD in lotmap 
					
					auto lotIter = lotMap.find(keyOfLotMap);

					if(lotIter != lotMap.end())
						tempDS.lot = lotIter->second;	
					else if(symbol.substr(0,monthIndex-4) == "NIFTY" || symbol.substr(0,monthIndex-4) == "BANKNIFTY" || symbol.substr(0,monthIndex-4) == "FINNIFTY")
					{
						int dateTemp = stoi(keyOfLotMap.substr(symbol.substr(0,monthIndex-4).length()));
						bool curMonthFlag = true;
						while(dateTemp%100<=31)
						{
							#ifdef debugDM
							cout << symbol.substr(0,monthIndex-4)+to_string(dateTemp) << '\n';
							#endif
							if(lotMap.find(symbol.substr(0,monthIndex-4)+to_string(dateTemp))!=lotMap.end())
							{	
								tempDS.lot = lotMap[keyOfLotMap] = lotMap[symbol.substr(0,monthIndex-4)+to_string(dateTemp)];
								break;
							}
							dateTemp++;
							if(dateTemp%100==32 && curMonthFlag)
							{
								curMonthFlag = false;
								dateTemp = (dateTemp/100)*100 + 101;
							}
						}
					}
		 		}	

	    	}
	    	else if((symbol).back() == 'T')
	    	{
	    		symbol = symbol + ".NFO";			// to make it similar to MINUTELY data, end it with .NFO just for getting expiry and lot
	    		tempDS.expiry = getFutureExpiry(symbol);					// cpoied afrom minutely 
	    		tempDS.lot = getLotSize(symbol,tempDS.expiry);				// copied from minutely
	    	}
	    }

    	(*data)[*currentSymbol] = tempDS;								// pushing in Datamap

		#ifdef debugDM
		cout << "Reading SNAPSHOT data in DM: " << *currentSymbol << " " << tempDS.open << " " << tempDS.high << " " << tempDS.low << " " << tempDS.close 
		<< " v" << tempDS.volume <<" LTP" << tempDS.LTP << " LTQ" << tempDS.LTQ << " " <<  " LTT" << tempDS.LTT << " ATP" << tempDS.ATP <<
		" bvec" << tempDS.buyDepth[0].first << ":" << tempDS.buyDepth[0].second << " " << tempDS.buyDepth[1].first << ":" << tempDS.buyDepth[1].second << " " << tempDS.buyDepth[2].first
		<< ":" << tempDS.buyDepth[2].second << " " << tempDS.buyDepth[3].first << ":" << tempDS.buyDepth[3].second << " " << tempDS.buyDepth[4].first << ":" << tempDS.buyDepth[4].second
		<< " svec" << tempDS.sellDepth[0].first << ":" << tempDS.sellDepth[0].second << " " << tempDS.sellDepth[1].first << ":" << tempDS.sellDepth[1].second << " " << 
		tempDS.sellDepth[2].first << ":" << tempDS.sellDepth[2].second << " " << tempDS.sellDepth[3].first << ":" << tempDS.sellDepth[3].second << " " << tempDS.sellDepth[4].first
		<< ":" << tempDS.sellDepth[4].second << endl;
		#endif

	    return true;
	}

    return false;
}

void dataManager::convertPipedDataToVector(vector<pair<int,int> > &v,string s1,string s2)
{
	stringstream ss1(s1);
	stringstream ss2(s2);

	while(ss1.good() && ss2.good())
	{
		string substr1,substr2;
		getline(ss1,substr1,'|');
		getline(ss2,substr2,'|');
		v.push_back(make_pair(stoi(substr1),stoi(substr2)));
	}
}

bool dataManager::decodeTBTData(string tempString,long long &time)
{
	index = 0;
    stringstream ss(tempString);
 	
    while (ss.good()) {
		getline(ss, substrTemp, ',');
        vTemp[index] = (substrTemp);
        index++;
    }

	if(index!=10)		// if input dataSeperated Strings are size 10, then its valid line to read
		return false;		// corrupt line

    string symbol;
    if(vTemp[3] == "N" || vTemp[3] == "M" || vTemp[3] == "X")	// ignoring ... || v[3] == "G" || v[3] == "H" || v[3] == "J"
    	symbol = vTemp[6];
    else if(vTemp[3] == "T")			// if message of TradeType and // ignoring ... || v[3] == "K"
    {
    	if(stoll(vTemp[5])==0 || stoll(vTemp[6])==0)	// if any of the orderId is 0 ,, it will be IOC order
    		symbol = vTemp[7];								
    	else
    		return false;									// // ignoring if tradeType and not IOC
    }

    if(validSymbols.find(symbol)!=validSymbols.end())
    {
    	DataSt tempDS;	
    	// tempDS.symbol = symbol;

    	if(vTemp[3] == "T")				// for IOC
		{		
	    	tempDS.messageType = 'I';		// putting  I for IOC order
	    	tempDS.orderID = 0;				// orderID will be 0 for IOC
	    	if(stoll(vTemp[5])==0)
				tempDS.side = 'B';				// converting 1 charater string to character
			else
				tempDS.side = 'S';
		} 
		else						// for M,M,X
		{
		   	tempDS.messageType = vTemp[3][0];		// N,M,X etc (first character of string)
	    	tempDS.orderID = stoll(vTemp[5]);
	    	tempDS.side = vTemp[7][0];				// converting 1 charater string to character
	    }

    	tempDS.time = stoll(vTemp[4]);
    	time = tempDS.time;

    	tempDS.price = stoll(vTemp[8]);
    	tempDS.quantity = stoi(vTemp[9]);
		tempDS.lot = 1;			/*** change this variable later later ***/	

		if(DataType == "FO")				// if FO data , we need to add extra fields
		{
			vector<string> symbolSeperated; 						// contains comma seperated strings
		    stringstream ss(symbol);
		 	
		    while (ss.good()) {
		        string substr;
		        getline(ss, substr, '_');
		        symbolSeperated.push_back(substr);
		    }

			if((symbol).back() == 'E')				// if options (not FUT)
	    	{
	    		tempDS.callput = symbolSeperated[3];
	    		tempDS.strike = stoi(symbolSeperated[2]);
			}

			//below statements are for both options and Futures
    		tempDS.expiry = symbolSeperated[1];
			string exp = convertEpochToDate(symbolSeperated[1]); // epoch ti expiry in DDMMMYY format
			
			string keyOfLotMap = symbolSeperated[0]+"20"+exp.substr(5,2)+monthNumber[exp.substr(2,3)]+exp.substr(0,2);
			//  convert DDMMMYY to YYYYMMDD abd then mathcing symbol + YYYYMMDD in lotmap 
			
			auto lotIter = lotMap.find(keyOfLotMap);

			if(lotIter != lotMap.end())
				tempDS.lot = lotIter->second;	
			else if(symbolSeperated[0] == "NIFTY" || symbolSeperated[0] == "BANKNIFTY" || symbolSeperated[0] == "FINNIFTY")
			{
				int dateTemp = stoi(keyOfLotMap.substr(symbolSeperated[0].length()));
				bool curMonthFlag = true;
				while(dateTemp%100<=31)
				{
					#ifdef debugDM
					cout << symbolSeperated[0]+to_string(dateTemp) << '\n';
					#endif
					if(lotMap.find(symbolSeperated[0]+to_string(dateTemp))!=lotMap.end())
					{	
						tempDS.lot = lotMap[keyOfLotMap] = lotMap[symbolSeperated[0]+to_string(dateTemp)];
						break;
					}
					dateTemp++;
					if(dateTemp%100==32 && curMonthFlag)
					{
						curMonthFlag = false;
						dateTemp = (dateTemp/100)*100 + 101;
					}
				}
			}
		}
    	
    	auto iter = (*data).find(symbol);
    	if(iter != (*data).end())
	    	tempDS.LTP = iter->second.LTP;
	    
    	(*data)[symbol] = tempDS;

    	*currentSymbol = symbol;

    	#ifdef debugDM
		cout << "\nReading TBT data in DM: " << *currentSymbol << " " << tempDS.messageType << " " << tempDS.time << " "
		<< tempDS.orderID << " " << tempDS.side << " " << tempDS.price << " " << tempDS.quantity << " exp"<<tempDS.expiry << 
		" strike"<< tempDS.strike << " l" << tempDS.lot <<"\n";
		#endif

    	return true;
    }

    return false;
}

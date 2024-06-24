#ifndef __dataManager__
#define __dataManager__
#include <bits/stdc++.h>
#include <stdint.h>
#include "structures.h"
#include "config.h"
using namespace std;
#define debugDM

class dataManager{
public:
	dataManager(unordered_map<string,DataSt> &, string &, string, string);
	void loadStreamsFODummy(string);
	bool loadDataFile(ifstream *);
	void loadExpiryFile(string);
	void loadLotSizeFile(string);
	void loadContractFile(string);
	// void insertFixedValuesInMap();
	string convertEpochToDate(string);
	string convertDataMode1to2(string);
	string convertDataMode2to1(string,bool);
	string getFutureExpiry(string);
	int getLotSize(string,string);
	uint8_t fetchData(long long &);
	bool decodeMinutelyData(string &,long long &);
	bool insertMinutelyDatatoDB(vector<string>,long long &);
	bool decodeSnapshotData(string,long long &);
	void convertPipedDataToVector(vector<pair<int,int> > &,string,string);
	bool decodeTBTData(string,long long &);

	uint8_t fetchDataDummy(long long &);
	bool decodeTBTDummy(string,long long &);

private:
	unordered_map<string,string> tokenToSymbol;
	unordered_map<string,int> lotMap;
	unordered_map<string,DataSt> *data;
	ifstream *myFile;
	string *currentSymbol;
	string F1,F2,F3;
	bool readingFirstLine = true;
	bool dataConversionFlag = true;
	vector<string> monthName = { "DUMMY" , "JAN", "FEB", "MAR", "APR", "MAY", "JUN", "JUL", "AUG", "SEP", "OCT", "NOV", "DEC"};
	map<string,string> monthNumber    {
        { "JAN", "01" },
        { "FEB", "02" },
        { "MAR", "03" },
        { "APR", "04" },
        { "MAY", "05" },
        { "JUN", "06" },
        { "JUL", "07" },
        { "AUG", "08" },
        { "SEP", "09" },
        { "OCT", "10" },
        { "NOV", "11" },
        { "DEC", "12" }
    };

        int min_len_for_mode2_future_valid_symbol=14;                                                                   // X_20210101_FUT, minimum length should be 14
	vector<string> vTemp = vector<string> (100);// contains comma seperated strings
	int index;// index iterator for above vector
	string substrTemp;
	vector<ifstream *> fileRead;
	vector<DataSt> dataStreams = vector<DataSt> (8);
	vector<string> streamSymbols = vector<string> (8);
	vector<bool> usefulData = vector<bool> (8,false);
//	DataSt dataStream1,dataStream2;
	vector<long long> curExchangeTime;
	uint8_t streamRead;
	long long minExchangeTime;
};

#endif

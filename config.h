#ifndef __configCPP__
#define __configCPP__

#include <bits/stdc++.h>

//#define debugAll

#ifdef debugAll
#define debugDM
#define debugMM
#define debugOM
#define debugSM
#define debugStrat
#define debugTradeDump
#endif


using namespace std;

extern string DataFreq,ReqSymbols,DataType,Date,DataFilePath,ContractFilePath;
extern int MatchingRuleNumber,Slippage,ModeOfData,ModeWanted,MatchingBar,PercentageVolumeMatching;//MaxLengthOfInputData,ExecutionStartTime,ExecutionEndTime,SquareOffTime,MaxNumOfPastIndicatorValues,GapBetweenOrders,MaxPyramidSize,;
extern bool IsOptions,VolumeBasedMatching,OrderMatchingInMultBars,CheckSignalWhenSLOrderIsPending,InterDaysOn;
extern unordered_set<string> validSymbols;
extern long long OrderIDStartingNumber;//,OrderValue,SLtrade,SLsymbol,SLtotal,TrailingSL,BookProfit;

bool readConfigFile(string,string);


#endif

#include "config.h"

string DataFreq,ReqSymbols,DataType,Date,DataFilePath,ContractFilePath;
int MatchingRuleNumber,Slippage,ModeOfData,ModeWanted,MatchingBar,/*MaxLengthOfInputData,ExecutionStartTime,ExecutionEndTime,SquareOffTime,MaxNumOfPastIndicatorValues,GapBetweenOrders,MaxPyramidSize,*/PercentageVolumeMatching;
bool IsOptions,VolumeBasedMatching,OrderMatchingInMultBars,CheckSignalWhenSLOrderIsPending,InterDaysOn;
unordered_set<string> validSymbols;
long long OrderIDStartingNumber = 5000000000000000000;

bool readConfigFile(string ConfigFilePath,string dataFolderPath)
{
	ifstream myFile(ConfigFilePath);
	if(!myFile.is_open())
    {
    	cout << "error opening config data file, path given :" << ConfigFilePath << '\n';
        return false;
    }
    
    while(!myFile.eof()) 
    {
    	string key,value,tempString;
		getline(myFile,tempString);
        if(tempString[0] == '#')        // ignore the line
            continue;
		stringstream ss(tempString);
        getline(ss, key, '=');
        getline(ss, value, '=');
        // cout << key << " " << value << endl;
        if(key == "DATA")
        	DataFreq = value;
        else if(key == "DATE")
        	Date = value;
        else if(key == "MATCHINGRULE")
        	MatchingRuleNumber = stoi(value);
        else if(key == "SYMBOL")
        	ReqSymbols = value;
        else if(key == "SLIPPAGE")
        	Slippage = stoi(value);
        else if(key == "DATATYPE")
        	DataType = value;
        else if(key == "MODEWANTED")
        	ModeWanted = stoi(value);
        else if(key == "ISOPTIONS")
            IsOptions = stoi(value);
        else if(key == "MATCHINGBAR")
            MatchingBar = stoi(value);
        else if(key == "VOLUMEBASEDMACTHING")
            VolumeBasedMatching = stoi(value);
        else if(key == "PERCENTAGEVOLUMEMATCHING")
            PercentageVolumeMatching = stoi(value);
        else if(key == "ORDERMATCHINGINMULTBARS")
            OrderMatchingInMultBars = stoi(value);
        else if(key == "CHECKSIGNALWHENSLORDERISPENDING")
            CheckSignalWhenSLOrderIsPending = stoi(value);
        else if(key == "INTERDAYSON")
            InterDaysOn = stoi(value);
	}
	cout << "Reading Config ** " << DataFreq << " " << Date << " Rule:" << MatchingRuleNumber << " " << ReqSymbols << 
	" Slippage:" << Slippage << " " << DataType  << " ModeWanted:" << ModeWanted << " isOptions:" << IsOptions << " matchingBar"
	<< MatchingBar << " VolumeBased:" << VolumeBasedMatching << " PercentageVolumeMatching:" << PercentageVolumeMatching << 
	" OrderMatchingInMultBars:" << OrderMatchingInMultBars << " CheckSignalWhenSLOrderIsPending:" << CheckSignalWhenSLOrderIsPending
	<< " InterDaysOn:" << InterDaysOn << ' '; 

    if(DataFreq == "MINUTELY")
	{
		std::string folderNameInsideMINUTELY = DataType;
		if(DataType == "FO" && !IsOptions)								// if someone  wants to run on for only Future
			folderNameInsideMINUTELY = "FUT";
		
		DataFilePath = dataFolderPath + DataFreq + "/" + folderNameInsideMINUTELY + "/" + Date.substr(0,4) + "/" + DataType +"_"+ Date + "_MINUTELY.csv";
        std::cout<<"Reading from path"<<DataFilePath<<std::endl;
	}
    else if(DataFreq == "SNAPSHOT")
    {
        DataFilePath = "./dataFiles/" + DataFreq + "/" + Date.substr(0,4) + "/" + Date.substr(4,2)
        +Date.substr(6,2) + "/" + Date + ".txt";
        ContractFilePath = "./dataFiles/" + DataFreq + "/" + Date.substr(0,4) + "/" + Date.substr(4,2)
        +Date.substr(6,2) + "/contract" + Date + ".txt";
    }
    else if(DataFreq == "TBT")
    {
        DataFilePath = "./dataFiles/" + DataFreq + "/" + Date.substr(0,4) + "/" + Date.substr(4,2)
        +Date.substr(6,2) + "/Combined_" + DataType + ".txt";
    }

    cout << DataFilePath << endl << ContractFilePath << endl;
    myFile.close();

    if(DataFreq == "MINUTELY")
    {   
	 stringstream ss(ReqSymbols);                //  reading valid Symbols from config
        while (ss.good()) 
        {   
            string subStr;
            getline(ss, subStr, ',');

/*
            if(DataType == "FO")
            {   
                if(!(subStr.substr(max(0,(int)subStr.length()-3),2)=="_F" || subStr.substr(max(0,(int)subStr.length()-1),1)=="E" || subStr.substr(max(0,(int)subStr.length()-3),3)=="FUT"))
                	{cout<<"Symbols in Config are not correct\n";return false;}
            }   
            */
        }   
     }   
     return true;
}

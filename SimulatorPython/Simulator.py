import pandas as pd
import os
from Strategy import Strategy
from datetime import datetime, timedelta
import config
class Simulator:
    def __init__(self,configFilePath):
        self.startDate=config.simStartDate
        self.startDate=datetime.strptime(str(self.startDate),"%Y%m%d")
        self.endDate=config.simEndDate
        self.endDate=datetime.strptime(str(self.endDate),"%Y%m%d")
        self.symbols=config.symbols.split(",")
        self.readData()
        self.strategy=Strategy(self)
        self.startSimulation()
        self.slippage=0.0001
        self.currentPrice={}   # map from symbol to close price
        self.sellValue={}
        self.buyValue={}
        self.currQuantity={}
    def readData(self):
        self.df=pd.DataFrame()
        date=self.startDate
        data_folder="/home/ubuntu/data"
        print("ReadData",self.startDate,self.endDate,self.symbols)
        while date<=self.endDate:
            date_folder=os.path.join(data_folder,date.strftime("%Y%m%d"))
            for symbol in self.symbols:
                file_path=os.path.join(date_folder,symbol)
                
                if os.path.exists(file_path):
                    temp_df=pd.read_csv(file_path)
                    temp_df["Symbol"]=symbol
                    self.df=pd.concat([self.df,temp_df],ignore_index=True)
            date+=timedelta(days=1)
        self.df=self.df.sort_values(by='time')
        print(self.df)
        
        
    def startSimulation(self):
        self.df=self.df[:10]
        for row in self.df.iterrows():
            # 1. update map self.currentPrice
            #print(row["open"],row["close"]....
            self.strategy.onMarketData(row)

    def onOrder(self,symbol,side,quantity,price):
        # 1. order confirmation send to strategy 'N'
        # 2. calculate trade price = if side is buy then trade price 
        if side=='B'
            tradePrice=(1+self.slippage)*self.currentPrice[symbol]
        if side=='S'
            tradePrice=(1-self.slippage)*self.currentPrice[symbol]
        tradeValue=tradePrice*quantity
        # 3a. send trade confirmation to strategy 'T'
        # 3b. update current quantity
        # 4. update sellValue or buyValue Map
        if symbol not in self.sellValue:
            self.sellValue[symbol]=0.0
        #self.sellValue[symbol]+=tradeValue
        # 5. print pnl
        pass
    def printPnl(self):
        # 1. make a list with unique symbols of all keys in sellValue & buyValue
        # 2.  pnl = sellValue - buyValue - currQuantity*price
    def dumpPnl(self):
        pass

Simulator("config.txt")

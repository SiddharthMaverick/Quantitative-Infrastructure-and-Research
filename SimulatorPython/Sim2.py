import pandas as pd
import os
from Strategy2 import Strategy
from datetime import datetime, timedelta
import config

class Simulator:
    def __init__(self, configFilePath):
        self.startDate = config.simStartDate
        self.startDate = datetime.strptime(str(self.startDate), "%Y%m%d")
        self.endDate = config.simEndDate
        self.endDate = datetime.strptime(str(self.endDate), "%Y%m%d")
        self.symbols = config.symbols.split(",")
        self.readData()
        self.strategy = Strategy(self)
        self.slippage = 0.0001
        self.currentPrice = {}
        self.sellValue = {}
        self.buyValue = {}
        self.currQuantity = {}
        self.startSimulation()

    def readData(self):
        self.df = pd.DataFrame()
        date = self.startDate
        data_folder = "/home/ubuntu/data"
        print("ReadData", self.startDate, self.endDate, self.symbols)
        
        while date <= self.endDate:
            date_folder = os.path.join(data_folder, date.strftime("%Y%m%d"))
            for symbol in self.symbols:
                file_path = os.path.join(date_folder, symbol)

                if os.path.exists(file_path):
                    temp_df = pd.read_csv(file_path)
                    temp_df["Symbol"] = symbol
                    self.df = pd.concat([self.df, temp_df], ignore_index=True)
            date += timedelta(days=1)
        
        self.df = self.df.sort_values(by='time')
        print(self.df)

    def startSimulation(self):
        for index, row in self.df.iterrows():
            self.currentPrice[row['Symbol']] = row['close']
            self.strategy.onMarketData(row)
            self.printPnl()

    def onOrder(self, symbol, side, quantity, price):
        tradePrice = price
        if side == 'B':
            tradePrice = (1 + self.slippage) * self.currentPrice[symbol]
        elif side == 'S':
            tradePrice = (1 - self.slippage) * self.currentPrice[symbol]
        
        tradeValue = tradePrice * quantity

        if symbol not in self.currQuantity:
            self.currQuantity[symbol] = 0
        if side == 'B':
            self.currQuantity[symbol] += quantity
            if symbol not in self.buyValue:
                self.buyValue[symbol] = 0.0
            self.buyValue[symbol] += tradeValue
        elif side == 'S':
            self.currQuantity[symbol] -= quantity
            if symbol not in self.sellValue:
                self.sellValue[symbol] = 0.0
            self.sellValue[symbol] += tradeValue

        self.strategy.onTradeConfirmation(symbol, side, quantity, tradePrice)

    def printPnl(self):
        pnl = 0.0
        symbols = set(self.sellValue.keys()).union(set(self.buyValue.keys()))
        
        for symbol in symbols:
            sellValue = self.sellValue.get(symbol, 0.0)
            buyValue = self.buyValue.get(symbol, 0.0)
            quantity = self.currQuantity.get(symbol, 0)
            currentPrice = self.currentPrice.get(symbol, 0.0)
            pnl += (sellValue - buyValue) + (quantity * currentPrice)
        
        print(f"PnL: {pnl}")

    def dumpPnl(self):
        self.printPnl()

if __name__ == "__main__":
    Simulator("config.txt")

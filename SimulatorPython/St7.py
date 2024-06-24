import config
from datetime import datetime

class Strategy:
    def __init__(self, simulator):
        self.sim = simulator
        self.x = 0
        self.closePrices = {}
        self.openPositions = []
        self.stopLoss = 2000
        self.takeProfit = 2000
        self.initialPrice = 0
        self.netPnL = 0
        self.lastCheckedTime = None
        self.totalPremiumReceived = 0

    def onMarketData(self, marketData):
        print("MarketDataReceived", marketData)
        symbol = marketData["Symbol"]

        if symbol not in self.closePrices:
            self.closePrices[symbol] = []
        self.closePrices[symbol].append(marketData["close"])

        current_time = datetime.fromtimestamp(marketData["time"])

        # Check futures price at 12:05 am on 19th March
        target_time = datetime.strptime("2024-03-19 00:05:00", "%Y-%m-%d %H:%M:%S")
        expiry_date = datetime.strptime("2024-03-20 12:00:00", "%Y-%m-%d %H:%M:%S")

        print("Market Data Time ", current_time, "    ", target_time)

        if current_time >= target_time and self.x == 0 and marketData["Symbol"] == "MARK:BTCUSDT":
            self.x = marketData["close"]
            self.initialPrice = self.x
            put_strike = self.find_closest_strike(self.x * 0.999)
            call_strike = self.find_closest_strike(self.x * 1.001)

            print("put_strike", put_strike)
            print("Call Strike ", call_strike)
            print("&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&")

            date_suffix = expiry_date.strftime("%d%m%y")
            symbol_p = f"MARK:P-BTC-{put_strike}-{date_suffix}"
            symbol_c = f"MARK:C-BTC-{call_strike}-{date_suffix}"

            print(marketData["Symbol"], "    ", symbol_p, "     ", symbol_c)
            print("&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&")

            self.sendOrderToExchange(symbol_p, 'S', 0.1, put_strike)
            self.sendOrderToExchange(symbol_c, 'S', 0.1, call_strike)
            self.openPositions.append({'side': 'S', 'quantity': 0.1, 'strike': put_strike, 'Symbol': symbol_p})
            self.openPositions.append({'side': 'S', 'quantity': 0.1, 'strike': call_strike, 'Symbol': symbol_c})

            self.lastCheckedTime = current_time

        # From 12:05 am 19th March till 12 pm 20th March: Check exit conditions
        if self.x != 0 and current_time < expiry_date and marketData["Symbol"] == "MARK:BTCUSDT":
            if marketData["close"] > self.x * 1.002 or marketData["close"] < self.x * 0.998:
                print("x : ", self.x, " close: ", marketData["close"])
                self.closePositions(marketData["close"])

        # Check PnL for stop loss and take profit
        self.calculatePnL(self.sim.currentPrice)
        if (abs(self.netPnL) >= self.stopLoss or abs(self.netPnL) >= self.takeProfit) and marketData["Symbol"] == "MARK:BTCUSDT":
            print("netPnl HIT : ", self.netPnL, " stoploss : ", self.stopLoss, " take Profit: ", self.takeProfit)
            self.closePositions(marketData["close"])

    def sendOrderToExchange(self, symbol, side, quantity, price):
        self.sim.onOrder(symbol, side, quantity, price)
        print("SendOrder", symbol, side, quantity, price)

    def onOrderResponse(self, responseType, symbol, side, quantity, price):
        print(f"Order Response - Type: {responseType}, Symbol: {symbol}, Side: {side}, Quantity: {quantity}, Price: {price}")

    def onTradeConfirmation(self, symbol, side, quantity, price):
        if side == 'S':
            self.totalPremiumReceived += price * quantity
        print(f"Trade Confirmation - Symbol: {symbol}, Side: {side}, Quantity: {quantity}, Price: {price}")

    def find_closest_strike(self, target_price):
        strikes = [int(x.split("-")[2]) for x in config.symbols if len(x.split("-")) > 3]
        closest_strike = min(strikes, key=lambda x: abs(x - target_price))
        return closest_strike

    def closePositions(self, current_price):
        for pos in self.openPositions:
            symbol = pos["Symbol"]
            quant = pos["quantity"]
            if pos["side"] == 'S':
                quant = -quant
            if quant > 0:
                self.sendOrderToExchange(symbol, 'S', abs(quant), current_price)
            else:
                self.sendOrderToExchange(symbol, 'B', abs(quant), current_price)
        self.openPositions = []

    def calculatePnL(self, currentPrice):
        current_value = sum([(currentPrice.get(pos["Symbol"], 0.0)) * pos['quantity'] for pos in self.openPositions])
        self.netPnL = self.totalPremiumReceived - current_value

#!usr/env/python
class Strategy:
    def __init__(self, simulator):
        self.sim = simulator
        self.x = 0
        self.closePrices = []

    def onMarketData(self, marketData):
        print("MarketDataReceived", marketData)
        self.closePrices.append(marketData["close"])
        
        if len(self.closePrices) >= 25:
            short_ema = self.ema(self.closePrices, 16)
            long_ema = self.ema(self.closePrices, 25)
            
            if short_ema > long_ema:
                self.sendOrderToExchange(marketData["Symbol"], 'B', 0.1, marketData["close"])
            elif short_ema < long_ema:
                self.sendOrderToExchange(marketData["Symbol"], 'S', 0.1, marketData["close"])

    def sendOrderToExchange(self, symbol, side, quantity, price):
        self.sim.onOrder(symbol, side, quantity, price)
        print("SendOrder", symbol, side, quantity, price)

    def onOrderResponse(self, responseType, symbol, side, quantity, price):
        print(f"Order Response - Type: {responseType}, Symbol: {symbol}, Side: {side}, Quantity: {quantity}, Price: {price}")

    def ema(self, prices, window):
        if len(prices) < window:
            return sum(prices) / len(prices)
        multiplier = 2 / (window + 1)
        ema_values = [sum(prices[:window]) / window]
        for price in prices[window:]:
            ema_values.append((price - ema_values[-1]) * multiplier + ema_values[-1])
        return ema_values[-1]

    def onTradeConfirmation(self, symbol, side, quantity, price):
        print(f"Trade Confirmation - Symbol: {symbol}, Side: {side}, Quantity: {quantity}, Price: {price}")

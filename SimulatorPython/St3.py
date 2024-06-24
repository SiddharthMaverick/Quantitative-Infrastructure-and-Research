import pandas as pd
import logging
import os
import csv
from datetime import datetime
import config


class Strategy:
    def __init__(self, simulator):
        self.sim = simulator
        self.entry_price = None
        self.put_strike_price = None
        self.call_strike_price = None
        self.put_position = False
        self.call_position = False
        self.profit_loss = 0
        self.closePrices = []

        logging.basicConfig(level=logging.INFO)
        self.logger = logging.getLogger(__name__)

        self.futurecurrent = self.checkFuturePrice("20240319", "MARK:BTCUSDT", "1pm")
        self.strikes = [float(x.split("-")[2]) for x in config.symbols if len(x.split("-")) > 3]

    def onMarketData(self, marketData):
        current_time = datetime.fromtimestamp(marketData['time'])
        current_price = self.futurecurrent
        closest_price = self.find_closest_strike(current_price)
        self.closePrices.append(closest_price)

        if self.entry_price is None and current_time >= datetime.strptime('2024-03-19 13:00:00', '%Y-%m-%d %H:%M:%S'):
            self.initialize_strategy(current_time, closest_price)
        elif self.entry_price is not None and current_time <= datetime.strptime('2024-03-20 11:59:00', '%Y-%m-%d %H:%M:%S'):
            if self.check_exit_conditions(closest_price):
                self.exit_positions(closest_price)

    def checkFuturePrice(self, date, symbol, target_time):
        folder_path = f"/home/ubuntu/data1/{date}"
        file_path = os.path.join(folder_path, f"{symbol}.csv")

        if not os.path.exists(file_path):
            self.logger.error(f"CSV file for {symbol} on {date} does not exist.")
            return None

        target_datetime = datetime.strptime(f"{date} {target_time}", "%Y%m%d %I%p")
        target_timestamp = int(target_datetime.timestamp())
        future_price = None

        with open(file_path, 'r') as csvfile:
            reader = csv.DictReader(csvfile)
            for row in reader:
                row_timestamp = int(row['time'])
                if row_timestamp == target_timestamp:
                    future_price = float(row['close'])
                    break

        if future_price:
            self.logger.info(f"Future price at {target_time} on {date} is {future_price}")
        else:
            self.logger.warning(f"No future price data available at {target_time} on {date}")

        return future_price

    def initialize_strategy(self, target_time, futures_price):
        self.entry_price = futures_price
        target_put_strike = futures_price * 0.98
        target_call_strike = futures_price * 1.02

        self.put_strike_price = self.find_closest_strike(target_put_strike)
        self.call_strike_price = self.find_closest_strike(target_call_strike)
        self.sell_option('put', self.put_strike_price, 0.1)
        self.sell_option('call', self.call_strike_price, 0.1)

    def find_closest_strike(self, target_strike):
        closest_strike = min(self.strikes, key=lambda x: abs(x - target_strike))
        return closest_strike

    def sell_option(self, option_type, strike_price, quantity):
        firstch = option_type[0].upper()
        strike_p = int(strike_price)
        symbol = f"MARK:{firstch}-BTC-{strike_p}-{config.simStartDate}"
        side = 'S'
        self.sim.onOrder(symbol, side, quantity, strike_price)
        if option_type == 'put':
            self.put_position = True
        elif option_type == 'call':
            self.call_position = True
        self.logger.info(f"Sell {option_type.capitalize()} Option: Strike Price = {strike_price}, Quantity = {quantity}")

    def check_exit_conditions(self, current_price):
        exit_positions = False

        if self.entry_price is not None:
            if current_price > self.entry_price * 1.01 or current_price < self.entry_price * 0.99:
                self.logger.info("Futures price moved beyond 1% threshold, exiting positions.")
                exit_positions = True

            if abs(self.profit_loss) >= 500:
                self.logger.info(f"Reached {'Stop Loss' if self.profit_loss <= -500 else 'Take Profit'}, exiting positions.")
                exit_positions = True

        return exit_positions

    def exit_positions(self, price):
        if self.put_position:
            firstch = 'P'
            strike_p = int(self.put_strike_price)
            symbol = f"MARK:{firstch}-BTC-{strike_p}-{config.simStartDate}"
            self.sim.onOrder(symbol, 'B', 0.1, self.put_strike_price)
            self.put_position = False
        if self.call_position:
            firstch = 'C'
            strike_p = int(self.call_strike_price)
            symbol = f"MARK:{firstch}-BTC-{strike_p}-{config.simStartDate}"
            self.sim.onOrder(symbol, 'B', 0.1, self.call_strike_price)
            self.call_position = False
        self.logger.info("Exiting all positions at current price.")

    def onTradeConfirmation(self, symbol, side, quantity, price):
        trade_value = price * quantity
        if side == 'S':
            self.profit_loss += trade_value
        elif side == 'B':
            self.profit_loss -= trade_value
        self.logger.info(f"Trade Confirmation: {side} {quantity} of {symbol} at {price}. Updated PnL: {self.profit_loss}")

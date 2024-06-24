import pandas as pd
import logging

class Strategy:
    def __init__(self, simulator):
        self.sim = simulator
        self.entry_price = None
        self.put_strike_price = None
        self.call_strike_price = None
        self.put_position = False
        self.call_position = False
        self.profit_loss = 0

        logging.basicConfig(level=logging.INFO)
        self.logger = logging.getLogger(__name__)

    def read_data(self, file_path):
        self.data = pd.read_csv(file_path, parse_dates=['time'])
        self.data.set_index('time', inplace=True)

    def initialize_strategy(self, target_time):
        futures_price = self.data.loc[target_time, 'close']
        self.entry_price = futures_price
        self.put_strike_price = round(futures_price * 0.98)
        self.call_strike_price = round(futures_price * 1.02)

        self.sell_option('put', self.put_strike_price, 0.1)
        self.sell_option('call', self.call_strike_price, 0.1)

    def sell_option(self, option_type, strike_price, quantity):
        side = 'S'
        self.sim.onOrder(option_type, side, quantity, strike_price)
        if option_type == 'put':
            self.put_position = True
        elif option_type == 'call':
            self.call_position = True
        self.logger.info(f"Sell {option_type.capitalize()} Option: Strike Price = {strike_price}, Quantity = {quantity}")

    def check_exit_conditions(self, current_price):
        exit_positions = False

        if current_price > self.entry_price * 1.01 or current_price < self.entry_price * 0.99:
            self.logger.info("Futures price moved beyond 1% threshold, exiting positions.")
            exit_positions = True

        if abs(self.profit_loss) >= 500:
            self.logger.info(f"Reached {'Stop Loss' if self.profit_loss <= -500 else 'Take Profit'}, exiting positions.")
            exit_positions = True

        return exit_positions

    def onMarketData(self, marketData):
        current_time = marketData['time']
        current_price = marketData['close']

        if self.entry_price is None and current_time >= pd.Timestamp('2024-03-19 13:00:00'):
            self.initialize_strategy(current_time)
        elif current_time <= pd.Timestamp('2024-03-20 12:00:00'):
            if self.check_exit_conditions(current_price):
                self.exit_positions(current_price)

    def exit_positions(self, price):
        if self.put_position:
            self.sim.onOrder('put', 'B', 0.1, self.put_strike_price)
            self.put_position = False
        if self.call_position:
            self.sim.onOrder('call', 'B', 0.1, self.call_strike_price)
            self.call_position = False
        self.logger.info(f"Exiting all positions at price")

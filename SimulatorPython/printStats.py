import pandas as pd
import numpy as np
import matplotlib.pyplot as plt

# Read and preprocess the PnL data
pnl_data = pd.read_csv('pnl_output.csv', parse_dates=[0], date_format='%Y-%m-%d %H:%M:%S', index_col=0)
pnl_data.dropna(subset=['PnL'], inplace=True)
pnl_data['Returns'] = pnl_data['PnL'].pct_change().replace([np.inf, -np.inf], np.nan).fillna(0)

# Calculate statistics
mean_pnl = pnl_data['PnL'].mean()
median_pnl = pnl_data['PnL'].median()
std_pnl = pnl_data['PnL'].std()
mean_return = pnl_data['Returns'].mean()
std_return = pnl_data['Returns'].std()

# Calculate cumulative PnL and drawdown
pnl_data['Cumulative PnL'] = pnl_data['PnL'].cumsum()
running_max = pnl_data['Cumulative PnL'].cummax()
drawdown = pnl_data['Cumulative PnL'] - running_max
max_drawdown = drawdown.min()

# Calculate risk measures
var_95 = pnl_data['Returns'].quantile(0.05)
es_95 = pnl_data['Returns'][pnl_data['Returns'] <= var_95].mean()

# Print statistics
print('Mean PnL:', mean_pnl)
print('Median PnL:', median_pnl)
print('Std Dev PnL:', std_pnl)
print('Mean Return:', mean_return)
print('Std Dev Return:', std_return)
print('Max Drawdown:', max_drawdown)
print('Value at Risk (95%):', var_95)
print('Expected Shortfall (95%):', es_95)

# Plotting
plt.figure(figsize=(14, 7))

# Plot Cumulative PnL
plt.subplot(2, 1, 1)
plt.plot(pnl_data.index, pnl_data['Cumulative PnL'], label='Cumulative PnL')
plt.title('Cumulative PnL')
plt.legend()

# Plot Drawdown
plt.subplot(2, 1, 2)
plt.plot(pnl_data.index, drawdown, label='Drawdown', color='red')
plt.title('Drawdown')
plt.legend()

plt.tight_layout()
plt.show()

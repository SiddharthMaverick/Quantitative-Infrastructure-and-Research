import config 

strikes=[int(x.split("-")[2]) for x in config.symbols if len(x.split("-"))>3]
print(strikes)

compiling all:
	g++ -shared -fPIC -g -std=c++11 config.cpp dataManager.cpp tradesDump.cpp orderBook.cpp orderManager.cpp simulatorManager.cpp matchingManager.cpp main.cpp -o libSimLibTest.so

common:
	g++-4.9 ./src/common/Net.cpp -std=c++11 -I ./src/common/ -c -o common.o
	ar rvs common.a common.o
	rm common.o

server: common
	g++-4.9 -I ./src/server -I ./src/common common.a src/server/Game.cpp  src/server/GameClient.cpp  src/server/GameServer.cpp  src/server/main.cpp -std=c++11 -o server


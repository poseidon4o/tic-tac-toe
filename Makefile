CC = clang++

all: server

common:
	$(CC) ./src/common/Net.cpp -g -std=c++11 -I ./src/common/ -c -o common.o
	ar rvs libcommon.a common.o
	rm common.o

server: common
	$(CC) -g -I ./src/server -I ./src/common src/server/Game.cpp  src/server/GameClient.cpp  src/server/GameServer.cpp  src/server/main.cpp -std=c++11 -L./ -lcommon -o server


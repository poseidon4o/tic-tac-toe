CC = g++-4.9

all: server

common:
	$(CC) ./src/common/Net.cpp ./src/common/Game.cpp -g -std=c++11 -I ./src/common/ -c -o common.o
	ar rvs libcommon.a common.o
	rm common.o

server: common
	$(CC) -I ./src/server -I ./src/common src/server/GameClient.cpp  src/server/GameServer.cpp  src/server/main.cpp -std=c++11 -L./ -lcommon -o server


ai_client: common
	$(CC) -I ./src/ai_client -I ./src/common ./src/ai_client/AiClient.cpp ./src/ai_client/main.cpp -std=c++11 -L./ -lcommon -o ai_client

text_client: common
	$(CC) -I ./src/common ./src/text_client/main.cpp -std=c++11 -L./ -lcommon -o text_client

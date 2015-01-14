CC = g++-4.9

all: server ai_client text_client

common:
	$(CC) ./src/common/Net.cpp -std=c++11 -I ./src/common/ -c -o net.o
	$(CC) ./src/common/Game.cpp -std=c++11 -I ./src/common/ -c -o game.o
	ar rvs libcommon.a game.o net.o

server: common
	$(CC) -I ./src/server -I ./src/common src/server/GameClient.cpp  src/server/GameServer.cpp  src/server/main.cpp -std=c++11 -L./ -lcommon -o server


ai_client: common
	$(CC) -I ./src/ai_client -I ./src/common ./src/ai_client/AiClient.cpp ./src/ai_client/main.cpp -std=c++11 -L./ -lcommon -o ai_client

text_client: common
	$(CC) -I ./src/common ./src/text_client/main.cpp -std=c++11 -L./ -lcommon -lpthread -o text_client

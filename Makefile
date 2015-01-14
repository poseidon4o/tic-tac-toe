CC = g++-4.9
BUILD_DIR = ./build/

all: server ai_client text_client

clean:
	rm $(BUILD_DIR)*

common:
	$(CC) ./src/common/Net.cpp -std=c++11 -I ./src/common/ -c -o $(BUILD_DIR)net.o
	$(CC) ./src/common/Game.cpp -std=c++11 -I ./src/common/ -c -o $(BUILD_DIR)game.o
	ar rvs $(BUILD_DIR)libcommon.a $(BUILD_DIR)game.o $(BUILD_DIR)net.o

server: common
	$(CC) -I ./src/server -I ./src/common src/server/GameClient.cpp  src/server/GameServer.cpp  src/server/main.cpp -std=c++11 -L$(BUILD_DIR) -lcommon -o $(BUILD_DIR)server


ai_client: common
	$(CC) -I ./src/ai_client -I ./src/common ./src/ai_client/AiClient.cpp ./src/ai_client/main.cpp -std=c++11 -L$(BUILD_DIR) -lcommon -o $(BUILD_DIR)ai_client

text_client: common
	$(CC) -I ./src/common ./src/text_client/main.cpp -std=c++11 -L$(BUILD_DIR) -lcommon -lpthread -o $(BUILD_DIR)text_client

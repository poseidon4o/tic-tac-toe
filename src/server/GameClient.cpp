#include "GameClient.h"

#include <string>
#include <sstream>
#include <iostream>
#include <cstring>
#include <vector>

using namespace std;

GameClient::GameClient(Socket sock): mSocket(std::move(sock)) {
}

GameClient::~GameClient() {
    mSocket.Close();
}

GameClient::operator bool() {
    return mSocket;
}

TextClient::TextClient(Socket sock): GameClient(std::move(sock)) {
}

TextClient::~TextClient() {
}

bool TextClient::SendData(const Game & game) {
    string data = "Current board:\n" + game.ToString();
    return mSocket.SendRetries(data.c_str(), data.size());
}

bool TextClient::SendPrompt() {
    const char * prompt = "\nNext move in form [x y]:\n";
    return mSocket.SendRetries(prompt, strlen(prompt));
}

bool TextClient::GetNextTurn(int & x, int & y) {
    char input[8];
    int received = sizeof(input) - 1;

    if (mSocket.RecvMax(reinterpret_cast<char*>(&input), received)) {
        input[received] = 0;
        stringstream strm(input);
        int ix, iy;
        if ((strm >> ix) && (strm >> iy)) {
            x = ix;
            y = iy;
            mSocket.ClearRecv();
            return true;
        }
    }

    return false;
}


BinaryClient::BinaryClient(Socket sock) : GameClient(std::move(sock)) {
}

BinaryClient::~BinaryClient() {
}

bool BinaryClient::SendPrompt() {
    return true;
}

bool BinaryClient::SendData(const Game & game) {
    vector<uint8_t> data;
    game.Serialize(data);
    return mSocket.SendRetries(reinterpret_cast<const char *>(data.data()), data.size());
}

bool BinaryClient::GetNextTurn(int & x, int & y) {
    char buff[sizeof(int) * 2] = { 0, };
    int received = sizeof(buff);
  
    if (mSocket.RecvMax(buff, received)) {
        if (received != sizeof(buff)) {
            return false;
        }
        mSocket.ClearRecv();
        x = static_cast<int>(*buff);
        y = static_cast<int>(*(buff + sizeof(int)));
        return true;
    }


    return 1;
}

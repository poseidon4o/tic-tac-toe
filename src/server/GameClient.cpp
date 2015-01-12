#include "GameClient.h"

#include <string>
#include <sstream>
#include <iostream>
#include <cstring>
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
    const char * prompt = "\nNext move in form [x, y]:\n";
    return mSocket.SendRetries(prompt, strlen(prompt));
}

bool TextClient::GetNextTurn(int & x, int & y) {
    char input[8];
    int received = sizeof(input) - 1;

    if (mSocket.RecvMax(reinterpret_cast<char*>(&input), received) && received) {
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
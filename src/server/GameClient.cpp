#include "GameClient.h"

#include <string>
#include <sstream>
using namespace std;

GameClient::GameClient(Client cl): cl(std::move(cl)) {
}

GameClient::~GameClient() {
    cl.Close();
}


TextClient::TextClient(Client cl): GameClient(std::move(cl)) {
}

TextClient::~TextClient() {
}

bool TextClient::SendData(const Game & game) {
    std::string data = "Current board:\n" + game.ToString() + "\nNext move in form [x, y]:\n";
    return cl.SendRetries(data.c_str(), data.size());
}

bool TextClient::GetNextTurn(int & x, int & y) {
    char input[8];

    if (cl.RecvRetries(reinterpret_cast<char*>(&input), sizeof(input))) {
        stringstream str(input);
        int ix, iy;
        if (str >> ix && str >> iy) {
            x = ix;
            y = iy;
            cl.ClearRecv();
            return true;
        }
    }

    return false;
}
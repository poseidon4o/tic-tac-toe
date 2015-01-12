#include "Game.h"
#include "GameServer.h"

#include <iostream>
using namespace std;


int main(int argc, char * argv[]) {
    GameServer server;
    server.run();

    return 0;
}
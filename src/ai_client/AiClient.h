#ifndef _AI_CLIENT_H_INCLUDED_
#define _AI_CLIENT_H_INCLUDED_

#include "../common/Game.h"
#include "../common/Net.h"

#include <string>

class AiClient {
    Client mSocket;
    Game mGame;
    Game::Color mMyColor;
    int mLevel;
    bool mInit;

    enum State { STARTING, SENDING, THINKING, RECEIVEING, FAIL } mState;

    union {
        struct {
            int mNextX, mNextY;
        };
        int mNextMove[2];
    };

    bool GetNextMove();
    bool ReceiveData();
public:
    AiClient(int level, const std::string & ip);

    bool Update();
};


#endif // _AI_CLIENT_H_INCLUDED_
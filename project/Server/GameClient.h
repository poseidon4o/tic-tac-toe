#ifndef _GAME_CLIENT_H_INCLUDED_
#define _GAME_CLIENT_H_INCLUDED_

#include "Game.h"

class GameClient {

public:

    virtual ~GameClient() = 0;

    virtual bool GetNextTurn() = 0;
    virtual bool SendBoard()
};

#endif // _GAME_CLIENT_H_INCLUDED_
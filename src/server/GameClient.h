#ifndef _GAME_CLIENT_H_INCLUDED_
#define _GAME_CLIENT_H_INCLUDED_

#include "../common/Net.h"
#include "Game.h"

class GameClient {
protected:
    Client cl;
public:
    GameClient(Client cl);
    
    virtual ~GameClient();

    virtual bool GetNextTurn(int & x, int & y) = 0;
    virtual bool SendData(const Game & game) = 0;
};



class TextClient: public GameClient {
public:
    TextClient(Client cl);
    ~TextClient() override;

    bool GetNextTurn(int & x, int & y) override;
    bool SendData(const Game & game) override;
};

#endif // _GAME_CLIENT_H_INCLUDED_
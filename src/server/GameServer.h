#ifndef _GAME_SERVER_H_INCLUDED_
#define _GAME_SERVER_H_INCLUDED_

#include "../common/Net.h"
#include "../common/Game.h"

#include "GameClient.h"

#include <vector>
#include <stack>
#include <memory>
#include <array>

typedef std::unique_ptr<GameClient> remote_cl;

class GamePair {
    std::array<remote_cl, 2> mPlayers;
    Game mGame;
    bool mSentData, mGetData;
public:
    GamePair(const GamePair &) = delete;
    GamePair & operator=(const GamePair &) = delete;

    GamePair() = delete;
    GamePair(remote_cl pl1, remote_cl pl2);

    GamePair(GamePair && gp);
    GamePair & operator=(GamePair && gp);

    ~GamePair();

    void Update();
    bool Finished() const;
};



class GameServer: protected Server {
    
    typedef std::array<remote_cl, 2> game_pair;

    std::vector<remote_cl> mWaiting;
    std::vector<GamePair> mPlaying;

    remote_cl getClient();

    GameServer(const GameServer &) = delete;
    GameServer & operator=(const GameServer &) = delete;

public:
    GameServer();

    void run();

};

#endif // _GAME_SERVER_H_INCLUDED_
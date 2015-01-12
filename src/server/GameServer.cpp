#include "GameServer.h"
#include <cassert>
#include <algorithm>

#include <chrono>
#include <thread>
#include <functional>

using namespace std;

GamePair::GamePair(remote_cl left, remote_cl right): mGame(Game::Color::X), mSentData(false) {
    mPlayers[0] = std::move(left);
    mPlayers[1] = std::move(right);
}

GamePair & GamePair::operator=(GamePair && gp) {
    mPlayers[0] = move(gp.mPlayers[0]);
    mPlayers[1] = move(gp.mPlayers[1]);
    mGame = gp.mGame;
    mSentData = gp.mSentData;
    return *this;
}

GamePair::GamePair(GamePair && gp) {
    mPlayers[0] = move(gp.mPlayers[0]);
    mPlayers[1] = move(gp.mPlayers[1]);
    mGame = gp.mGame;
    mSentData = gp.mSentData;
}

void GamePair::Update() {
    remote_cl & next = mPlayers[mGame.OnMove()];
    if (!mSentData) {
        next->SendData(mGame);
        mSentData = true;
    }

    int x, y;
    if (next->GetNextTurn(x, y)) {
        if (mGame.MakeMove(x, y)) {
            mSentData = false;
        }
        next->SendData(mGame);
    }
}


// TODO: check for client connection
bool GamePair::Finished() const {
    return mGame.GetWinner() != Game::Color::NONE;
}


GameServer::GameServer() {
    assert(Start(8090));
}

remote_cl GameServer::getClient() {
    Socket sock = Accept();
    if (sock) {
        return std::unique_ptr<TextClient>(new TextClient(std::move(sock)));
    }
    return std::unique_ptr<TextClient>(nullptr);
}

void GameServer::run() {
    for (;;) {
        remote_cl cl = getClient();
        if (cl) {
            mWaiting.push(std::move(cl));
        }

        while (mWaiting.size() >= 2) {
            remote_cl left = std::move(mWaiting.top());
            mWaiting.pop();
            remote_cl right = std::move(mWaiting.top());
            mWaiting.pop();

            mPlaying.push_back(GamePair(std::move(left), std::move(right)));
        }

        remove_if(mPlaying.begin(), mPlaying.end(), mem_fn(&GamePair::Finished));

        for_each(mPlaying.begin(), mPlaying.end(), mem_fn(&GamePair::Update));        

        this_thread::sleep_for(chrono::microseconds(1));
    }
}

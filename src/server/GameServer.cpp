#include "GameServer.h"
#include <cassert>
#include <algorithm>

#include <chrono>
#include <thread>
#include <functional>

using namespace std;

GamePair::GamePair(remote_cl left, remote_cl right) : mGame(Game::Color::X), mSentData(false), mGetData(false) {
    mPlayers[0] = std::move(left);
    mPlayers[1] = std::move(right);
}

GamePair & GamePair::operator=(GamePair && gp) {
    mPlayers[0] = move(gp.mPlayers[0]);
    mPlayers[1] = move(gp.mPlayers[1]);
    mGame = gp.mGame;
    mSentData = gp.mSentData;
    mGetData = gp.mGetData;
    return *this;
}

GamePair::GamePair(GamePair && gp) {
    mPlayers[0] = move(gp.mPlayers[0]);
    mPlayers[1] = move(gp.mPlayers[1]);
    mGame = gp.mGame;
    mSentData = gp.mSentData;
    mGetData = gp.mGetData;
}

void GamePair::Update() {
    remote_cl & next = mPlayers[mGame.OnMove()];

    if (!mSentData) {
        next->SendData(mGame);
        if (!mGame.Finished()) {
            next->SendPrompt();
        }
        mSentData = true;
    }

    int x, y;
    if (next->GetNextTurn(x, y)) {
        if (!mGetData) {
            mGetData = true;
            next->SendData(mGame);
        }
        if (mGame.MakeMove(x, y)) {
            mGetData = false;
            mSentData = false;
        } else if (!mGame.Finished()) {
            next->SendPrompt();
        }
    }
}


// TODO: check for client connection
bool GamePair::Finished() const {
    return mGame.Finished() || false == *mPlayers[0] || false == *mPlayers[1];
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
            mWaiting.push_back(std::move(cl));
        }

        while (mWaiting.size() >= 2) {
            remote_cl left = std::move(mWaiting.back());
            mWaiting.pop_back();
            remote_cl right = std::move(mWaiting.back());
            mWaiting.pop_back();

            mPlaying.push_back(GamePair(std::move(left), std::move(right)));
        }

        mPlaying.erase(remove_if(mPlaying.begin(), mPlaying.end(), mem_fn(&GamePair::Finished)), mPlaying.end());

        for_each(mPlaying.begin(), mPlaying.end(), mem_fn(&GamePair::Update));

        this_thread::sleep_for(chrono::microseconds(1));
    }
}

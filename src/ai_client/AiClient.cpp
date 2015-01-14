#include "AiClient.h"
#include <iostream>
#include <vector>
#include <cstdint>
#include <utility>
#include <limits>
#include <initializer_list>
#include <cassert>

using namespace std;

// TODO: unuglify!
static int evaluateLine(int row1, int col1, int row2, int col2, int row3, int col3, Game::Color player, Game::Color opp, const Game & game) {
    int score = 0;

    // First cell
    if (game[row1][col1] == player) {
        score = 1;
    } else if (game[row1][col1] == opp) {
        score = -1;
    }

    // Second cell
    if (game[row2][col2] == player) {
        if (score == 1) {   // cell1 is mySeed
            score = 10;
        } else if (score == -1) {  // cell1 is oppSeed
            return 0;
        } else {  // cell1 is empty
            score = 1;
        }
    } else if (game[row2][col2] == opp) {
        if (score == -1) { // cell1 is oppSeed
            score = -10;
        } else if (score == 1) { // cell1 is mySeed
            return 0;
        } else {  // cell1 is empty
            score = -1;
        }
    }

    // Third cell
    if (game[row3][col3] == player) {
        if (score > 0) {  // cell1 and/or cell2 is mySeed
            score *= 10;
        } else if (score < 0) {  // cell1 and/or cell2 is oppSeed
            return 0;
        } else {  // cell1 and cell2 are empty
            score = 1;
        }
    } else if (game[row3][col3] == opp) {
        if (score < 0) {  // cell1 and/or cell2 is oppSeed
            score *= 10;
        } else if (score > 1) {  // cell1 and/or cell2 is mySeed
            return 0;
        } else {  // cell1 and cell2 are empty
            score = -1;
        }
    }
    return score;
}

static int value(const Game & g, Game::Color player) {
    Game::Color opp = static_cast<Game::Color>(!player);
    int score = 0;

    // rows
    score += evaluateLine(0, 0, 0, 1, 0, 2, player, opp, g);
    score += evaluateLine(1, 0, 1, 1, 1, 2, player, opp, g); 
    score += evaluateLine(2, 0, 2, 1, 2, 2, player, opp, g);

    // cols
    score += evaluateLine(0, 0, 1, 0, 2, 0, player, opp, g);
    score += evaluateLine(0, 1, 1, 1, 2, 1, player, opp, g);
    score += evaluateLine(0, 2, 1, 2, 2, 2, player, opp, g);

    // main diag
    score += evaluateLine(0, 0, 1, 1, 2, 2, player, opp, g);
    // second diag 
    score += evaluateLine(0, 2, 1, 1, 2, 0, player, opp, g);

    return score;
}

static std::pair<int, int> minmax(const Game::Color ai, int depth, Game::Color player, const Game & g, int & outputScore) {
    outputScore = (player == ai) ? numeric_limits<int>::min() : numeric_limits<int>::max();
    int col = -1, row = -1, score = 0;

    if (g.Finished() || !depth) {
        outputScore = value(g, ai);
        return { col, row };
    }
    for (int c = 0; c < Game::Size; ++c) {
        for (int r = 0; r < Game::Size; ++r) {
            if (g[c][r] != Game::Color::NONE) {
                continue;
            }

            Game newg(g);
            newg.MakeMove(c, r);
            minmax(ai, depth - 1, static_cast<Game::Color>(!player), newg, score);

            if (player == ai) {
                if (score > outputScore) {
                    outputScore = score;
                    col = c;
                    row = r;
                }
            } else {
                if (score < outputScore) {
                    outputScore = score;
                    col = c;
                    row = r;
                }
            }
        }
    }
    return {col, row};
}



AiClient::AiClient(int level, const string & ip): mLevel(level), mInit(false), mMyColor(Game::Color::NONE), mState(State::STARTING) {
    if (mSocket.Connect(ip, 8090)) {
        uint64_t id = 0;
        if (mSocket.SendRetries(reinterpret_cast<const char *>(&id), sizeof(id))) {
            mState = State::RECEIVEING;
        } else {
            mState = State::FAIL;
        }
    } else {
        mState = State::FAIL;
    }
}

bool AiClient::ReceiveData() {
    uint8_t header;
    int recvSize = sizeof(header);
    if (!mSocket.RecvMax(reinterpret_cast<char *>(&header), recvSize)) {
        return false;
    }
    --header;

    vector<uint8_t> data;
    data.resize(header);
    recvSize = header;
    if (!mSocket.RecvMax(reinterpret_cast<char *>(data.data()), recvSize) || recvSize != header) {
        return false;
    }

    Game newGame;

    if (!Game::Deserialize(data, newGame)) {
        return false;
    }

    if (!mInit) {
        mGame = newGame;
        mMyColor = mGame.OnMove();
        return mInit = true;
    }

    if (mGame == newGame || mMyColor != newGame.OnMove()) {
        return false;
    }

    mGame = newGame;
    return true;
}

bool AiClient::GetNextMove() {
    int score;
    pair<int, int> r = minmax(mMyColor, mLevel, mMyColor, mGame, score);
    mNextX = r.first;
    mNextY = r.second;
    return true;
}

bool AiClient::Update() {
    if (false == mSocket || mGame.Finished()) {
        mState = FAIL;
    }

    switch (mState) {
    case AiClient::FAIL:
        return false;
    case AiClient::RECEIVEING:
        if (ReceiveData()) {
            mState = THINKING;
            return true;
        }
        return true;
    case AiClient::THINKING:
        if (GetNextMove()) {
            mState = SENDING;
            return true;
        }
        break;
    case AiClient::SENDING:
        if (mSocket.SendRetries(reinterpret_cast<const char *>(mNextMove), sizeof(mNextMove))) {
            mState = RECEIVEING;
            return true;
        }
    }
    mState = FAIL;
    return false;
}
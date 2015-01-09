#ifndef _GAME_H_INCLUDED_
#define _GAME_H_INCLUDED_

#include <array>
#include <cstdint>

const int GameSize = 3;

class Game {
    enum {Size = GameSize};
    enum Color: uint8_t { X, O, NONE };

    typedef std::array<Color, Game::Size> Row;

    std::array<Row, Game::Size> mBoard;
    Color mNextTurn, mWinner;
public:
    Game(Color first);

    Color GetWinner() const;

    bool MakeMove(int x, int y);

private:
    void checkWinner(int x, int y);
};


#endif // _GAME_H_INCLUDED_
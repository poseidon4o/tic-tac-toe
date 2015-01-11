#ifndef _GAME_H_INCLUDED_
#define _GAME_H_INCLUDED_

#include <array>
#include <cstdint>
#include <string>

const int GameSize = 3;

class Game {
public:
    enum Color: uint8_t { X, O, NONE };
    enum { Size = GameSize };

private:
    typedef std::array<Color, Game::Size> Row;

    std::array<Row, Game::Size> mBoard;
    Color mNextTurn, mWinner;
public:



    Game(Color first);

    Color GetWinner() const;

    bool MakeMove(int x, int y);

    std::string ToString() const;

private:
    void checkWinner(int x, int y);
};


#endif // _GAME_H_INCLUDED_
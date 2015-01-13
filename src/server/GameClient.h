#ifndef _GAME_CLIENT_H_INCLUDED_
#define _GAME_CLIENT_H_INCLUDED_

#include "../common/Net.h"
#include "../common/Game.h"

class GameClient {
protected:
    Socket mSocket;
public:
    GameClient(Socket sock);
    
    virtual ~GameClient();

    operator bool();

    virtual void SetColor(Game::Color c) = 0;
    virtual bool SendPrompt() = 0;
    virtual bool GetNextTurn(int & x, int & y) = 0;
    virtual bool SendData(const Game & game) = 0;
};



class TextClient: public GameClient {
public:
    TextClient(Socket sock);
    ~TextClient() override;

    void SetColor(Game::Color c) override;
    bool SendPrompt() override;
    bool GetNextTurn(int & x, int & y) override;
    bool SendData(const Game & game) override;
};



class BinaryClient : public GameClient {
public:
    BinaryClient(Socket sock);
    ~BinaryClient() override;

    void SetColor(Game::Color c) override;
    bool SendPrompt() override;
    bool GetNextTurn(int & x, int & y) override;
    bool SendData(const Game & game) override;
};

#endif // _GAME_CLIENT_H_INCLUDED_
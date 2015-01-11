#ifndef _NET_H_INCLUDED_
#define _NET_H_INCLUDED_

#include <string>

class Socket {
protected:
    friend class Server;
    int mSocket;

    Socket(int sockfd);
    Socket() = delete;
public:
    Socket(Socket && rval);

    ~Socket();

    Socket(const Socket &) = delete;
    Socket & operator=(const Socket &) = delete;

    operator bool();

    int Send(const char * data, int size);
    int Recv(char * data, int size);

    void Close();

    std::string GetPeerName();
};



class Server {
    int mSocket;
public:
    Server();
    ~Server();

    bool Start(int port);
    void Stop();

    Socket Accept();
};



class Client: public Socket {
public:
    Client();

    bool Connect(const std::string & ip, int port);
};


#endif // _NET_H_INCLUDED_

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

    bool SendRetries(const char * data, int size, int retries = 5);
    bool RecvRetries(char * data, int size, int retries = 5);

    void ClearRecv();

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
    Client(Client && cl);

    Client(const Client &) = delete;
    Client & operator=(const Client &) = delete;

    ~Client() = default;

    bool Connect(const std::string & ip, int port);
};


#endif // _NET_H_INCLUDED_

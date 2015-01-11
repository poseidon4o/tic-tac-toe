#ifndef _NET_H_INCLUDED_
#define _NET_H_INCLUDED_

#include <string>

class Socket {
    int mSocket;

    Socket(int sockfd);
public:
    ~Socket();

    Socket(const Socket &) = delete;
    Socket & operator=(const Socket &) = delete;


    int Send(const char * data, int size);
    int Recv(char * data, int size);

    void Close();

    std::string GetPeerName();   
};




#endif // _NET_H_INCLUDED_

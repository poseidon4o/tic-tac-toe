#include "Net.h"

#if defined(_WIN32)
#define C_WIN_SOCK
#include <winsock2.h>
#include <WS2tcpip.h>
#else
#define SOCKET_ERROR -1

#include <cstdlib>
#include <errno.h>
#include <cstring>
#include <fcntl.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/ioctl.h>
#endif

Socket::Socket(int sockfd): mSocket(sockfd) {
}

Socket::Socket(Socket && rval) {
    mSocket = rval.mSocket;
    rval.mSocket = -1;
}

Socket::~Socket() {
    Close();
}

int Socket::Send(const char * data, int size) {
    return mSocket != -1 ? send(mSocket, data, size, 0) : 0;
}

static bool wouldBlock() {
#ifdef C_WIN_SOCK
    int err = WSAGetLastError();
    return err && err == WSAEWOULDBLOCK;
#else
    return errno && (errno == EAGAIN || errno == EWOULDBLOCK);
#endif
}

bool Socket::SendRetries(const char * data, int size, int retries) {
    int sent = 0;
    bool block = false;
    do {
        int got = Send(data + sent, size - sent);
        sent += got;
        if (got == 0) {
            mSocket = -1;
            return false;
        } else if (got < 0) {
            block = wouldBlock();
            if (!block) {
                mSocket = -1;
                return false;
            }
        }
    } while (sent != size && block && --retries);

    return sent == size;
}

int Socket::Recv(char * data, int size) {
    return mSocket != -1 ? recv(mSocket, data, size, 0) : 0;
}

bool Socket::RecvMax(char * data, int & size) {
    bool block = false;
    size = Recv(data, size);
    if (size == 0) {
        mSocket = -1;
    } else if (size < 0) {
        block = wouldBlock();
        if (!block) {
            mSocket = -1;
            return false;
        }
    }
    return size > 0;
}

void Socket::ClearRecv() {
    char buff[256];
    int got = 0, retries = 10;
    do {
        got = Recv(buff, 256);
    } while (got && !wouldBlock() && --retries);
}

Socket::operator bool() {
    return mSocket != -1;
}

void Socket::Close() {
    if (mSocket != -1) {
#ifdef C_WIN_SOCK
        closesocket(mSocket);
#else
        close(mSocket);
#endif
        mSocket = -1;
    }
}

std::string Socket::GetPeerName() {
    if (mSocket == -1)
        return "";

    sockaddr_in clientSocket;
    socklen_t size = sizeof(sockaddr_in);
    getpeername(mSocket, reinterpret_cast<sockaddr*>(&clientSocket), &size);

    return std::string(inet_ntoa(clientSocket.sin_addr));
}


Server::Server(): mSocket(-1) {
#ifdef C_WIN_SOCK
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif
}

Server::~Server() {
    Stop();
}

static bool setNonBlocking(int sockfd) {
#ifdef C_WIN_SOCK
    u_long mode = 1;
    return ioctlsocket(sockfd, FIONBIO, &mode) == NO_ERROR;
#else
    return fcntl(sockfd, F_SETFL, O_NONBLOCK) != SOCKET_ERROR;
#endif
}

bool Server::Start(int port) {
    if (mSocket != -1) {
        return false;
    }

    mSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (mSocket == -1) {
        return false;
    }

    sockaddr_in desc;
    memset(&desc, 0, sizeof(desc));

    desc.sin_family = AF_INET;
    desc.sin_addr.s_addr = htonl(INADDR_ANY);
    desc.sin_port = htons(port);

    int status = bind(mSocket, reinterpret_cast<sockaddr*>(&desc), sizeof(desc));
    if (status < 0) {
        Stop();
        return false;
    }

    status = listen(mSocket, SOMAXCONN);
    if (status < 0) {
        Stop();
        return false;
    }

    if (!setNonBlocking(mSocket)) {
        Stop();
        return false;
    }

    return true;
}

void Server::Stop() {
    if (mSocket != -1) {
#ifdef C_WIN_SOCK
        closesocket(mSocket);
#else
        close(mSocket);
#endif
        mSocket = -1;
    }
}

Socket Server::Accept() {
    if (mSocket == -1) {
        return Socket(-1);
    }

    int sockfd = accept(mSocket, nullptr, nullptr);
    if (sockfd == -1) {
        return Socket(-1);
    }

    if (!setNonBlocking(sockfd)) {
        return Socket(-1);
    }

    return Socket(sockfd);
}

Client::Client(): Socket(-1) {
#ifdef C_WIN_SOCK
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif
}

bool Client::Connect(const std::string & ip, int port) {

    mSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (mSocket == -1) {
        return false;
    }

    sockaddr_in desc;
    memset(&desc, 0, sizeof(desc));
    desc.sin_family = AF_INET;
    desc.sin_port = htons(port);
    desc.sin_addr.s_addr = inet_addr(ip.c_str());

    if (connect(mSocket, reinterpret_cast<sockaddr*>(&desc), sizeof(desc)) != 0) {
        return false;
    }

    if (!setNonBlocking(mSocket)) {
        Close();
        return false;
    }

    return true;
}

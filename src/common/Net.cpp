#include "Net.h"

#if defined(_WIN32)
    #define C_WIN_SOCK
    #include <winsock2.h>
    #include <WS2tcpip.h>
#else
    #include <sys/socket.h>
    #include <sys/types.h>
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
    return mSocket != -1 ? send(mSocket, data, size, 0) : -1;
}

int Socket::Recv(char * data, int size) {
    return mSocket != -1 ? recv(mSocket, data, size, 0) : -1;
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
        return false;
    }

    status = listen(mSocket, SOMAXCONN);
    if (status < 0) {
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

static bool setNonBlocking(int sockfd) {
#ifdef C_WIN_SOCK
    u_long mode = 1;
    return ioctlsocket(sockfd, FIONBIO, &mode) == NO_ERROR;
#else
    return fcntl(sockfd, F_SETFL, O_NONBLOCK) != SOCKET_ERROR;
#endif
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

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

Socket::~Socket() {
    Close();
}

int Socket::Send(const char * data, int size) {
    return mSocket != -1 ? send(mSocket, data, size, 0) : -1;
}

int Socket::Recv(char * data, int size) {
    return mSocket != -1 ? recv(mSocket, data, size, 0) : -1;
}

void Socket::Close() {
    if (mSocket != -1) {
#ifdef C_WIN_SOCK
        closesocket(mSocket);
#else
        close(mSocket);
#endif // C_WIN_SOCK
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

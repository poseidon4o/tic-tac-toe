#include "../common/Net.h"

#include <cstring>
#include <string>
#include <iostream>
#include <thread>
#include <chrono>

using namespace std;

int main(int argc, char * argv[]) {
    string ip = "127.0.0.1";
    Client cl;
    if (!cl.Connect(ip, 8090)) {
        cout << "Failed to connect to " << ip << endl;
        return 0;
    }
    bool DC = false;

    thread reader([&cl, &DC]() mutable {
        char buff[1024];
        while (1) {
            if (cin.getline(buff, sizeof(buff) - 1)) {
                if (!cl.SendRetries(buff, strlen(buff))) {
                    DC = true;
                    return;
                }
            }
        }
    });

    char buff[8192];
    while (!DC) {
        int recvSize = sizeof(buff) - 1;
        if (cl.RecvMax(buff, recvSize)) {
            buff[recvSize] = 0;
            cout << buff;
        }
    }

    while (!reader.joinable()) {
        this_thread::sleep_for(chrono::milliseconds(1));
    }
    reader.join();

    cout << "Disconnected" << endl;
    return 0;
}
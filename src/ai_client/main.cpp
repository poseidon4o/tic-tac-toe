#include "AiClient.h"

#include <iostream>
#include <sstream>

using namespace std;


int main(int argc, char * argv[]) {
    int botCount = 9;
    if (argc == 2) {
        stringstream strm(argv[1]);
        strm >> botCount;
    }

    AiClient ** bots = new AiClient*[botCount];
    for (int c = 0; c < botCount; ++c) {
        bots[c] = NULL;
    }

    while (1) {
        for (int c = 0; c < botCount; ++c) {
            if (!bots[c] || !bots[c]->Update()) {
                delete bots[c];
                bots[c] = new AiClient(7, "127.0.0.1");
            }
        }
    }
    

    return 0;
}
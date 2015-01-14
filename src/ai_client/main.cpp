#include "AiClient.h"

#include <iostream>
#include <sstream>

using namespace std;


int main(int argc, char * argv[]) {
    int botCount = 9;

    if (argc < 2) {
        cout << "Usage\n\t" << argv[0] << " IP [bot-count=" << botCount << "]\n";
        return 0;
    } 

    string ip(argv[1]);

    if (argc == 3) {
        stringstream strm(argv[2]);
        strm >> botCount;
    }

    cout << "Running " << botCount << " Ai clients to server " << ip << endl;

    AiClient ** bots = new AiClient*[botCount];
    for (int c = 0; c < botCount; ++c) {
        bots[c] = NULL;
    }

    while (1) {
        for (int c = 0; c < botCount; ++c) {
            if (!bots[c] || !bots[c]->Update()) {
                delete bots[c];
                bots[c] = new AiClient(min(max(c, 1), 9), ip);
            }
        }
    }
    

    return 0;
}
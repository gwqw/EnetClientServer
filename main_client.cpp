#include <iostream>

#include <enet/enet.h>

#include "enet_client.h"

using namespace std;

int main() {
    if (enet_initialize() != 0) {
        cerr << "An error occurred while initializing ENet.\n";
        return EXIT_FAILURE;
    }
    atexit (enet_deinitialize);
    try {
        EnetClient client;
        if (client.connect("localhost", 9000)) {
            cout << "Connection to " << "localhost" << " succeeded.\n";
        }
        cout << "Input cmd:\n";
        for (string line; getline(cin, line);) {
            client.sendText(line);
            int n = stoi(line);
            client.sendData({n});
        }
        return 0;
    } catch (const std::exception& e) {
        cerr << e.what() << endl;
        return EXIT_FAILURE;
    }
}

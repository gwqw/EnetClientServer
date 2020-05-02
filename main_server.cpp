#include <iostream>

#include <enet/enet.h>

#include "enet_server.h"

using namespace std;

int main() {
    if (enet_initialize () != 0) {
        cerr << "An error occurred while initializing ENet.\n";
        return EXIT_FAILURE;
    }
    atexit (enet_deinitialize);
    std::cout << "Server is started" << std::endl;
    try {
        EnetServer enet_server(9000, 10);

        return 0;
    } catch (const exception& e) {
        cerr << e.what() << endl;
        return -1;
    }
}

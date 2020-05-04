#include <iostream>

#include <string>

#include <enet/enet.h>

#include "enet_server.h"

using namespace std;

void print_str(const std::string& str) {
    cout << str << '\n';
}

void print_data(const std::vector<int>& data) {
    for (auto n : data) {
        cout << n << ", ";
    }
    cout << '\n';
}

int main() {
    if (enet_initialize() != 0) {
        cerr << "An error occurred while initializing ENet.\n";
        return EXIT_FAILURE;
    }
    atexit (enet_deinitialize);
    try {
        EnetServer enet_server(9000, 10, print_str, print_data);

        return 0;
    } catch (const exception& e) {
        cerr << e.what() << endl;
        return -1;
    }
}

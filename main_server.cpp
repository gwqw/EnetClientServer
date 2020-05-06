#include <iostream>

#include <string>
#include <vector>
#include <cstring>

#include <enet/enet.h>

#include "enet_server.h"

using namespace std;

void print_str(const std::string& str) {
    cout << str << '\n';
}

void print_data(const std::vector<std::uint8_t>& data) {
    vector<int> tmp(data.size() / sizeof(int));
    memcpy((void*)tmp.data(), (void*)data.data(), data.size());

    for (auto n : tmp) {
        cout << n << ", ";
    }
    cout << '\n';
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: enet_server <port>\n";
        return 1;
    }
    try {
        EnetServer enet_server(stoi(argv[1]), 10, print_str, print_data);
        cerr << "server started" << endl;
        cout << "Enter quit for quit" << endl;
        string command;
        while (command != "quit") {
            cin >> command;
        }
        return 0;
    } catch (const exception& e) {
        cerr << e.what() << endl;
        return EXIT_FAILURE;
    }
}

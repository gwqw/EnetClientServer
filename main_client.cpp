#include <iostream>

#include <enet/enet.h>

#include "enet_client.h"

using namespace std;

int main() {
    EnetClient client("localhost", 9000);
    cout << "Input cmd:\n";
    for (string line; getline(cin, line); ) {
        client.sendText(line);
    }

    std::cout << "Hello, World!" << std::endl;
    return 0;
}

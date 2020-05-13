#include <iostream>

#include <enet/enet.h>

#include "enet_client.h"

using namespace std;

vector<uint8_t> getVector(const std::string& str) {
    vector<uint8_t> v;
    size_t pos = str.find(',');
    size_t start = 0;
    while (start != string::npos) {
        v.push_back(stoi(str.substr(start, pos - start)));
        if (pos == string::npos) break;
        start = pos+1;
        pos = str.find(',', start);
    }
    return v;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: enet_client <address> [<port>]\n"
                     "Default port number - 9000\n";
        return 1;
    }

    try {
        int port_num = 9000;
        if (argc >= 3) {
            port_num = stoi(argv[2]);
        }
        EnetClient client;
        if (client.connect(string(argv[1]), port_num)) {
            cerr << "Connection to " << argv[1] << " succeeded." << endl;
        }
        cout << "Input cmd (ctrl+D -- exit):\n";
        for (string line; getline(cin, line);) {
            if (line.size() > 2 && line[0] == 's' && line[1] == ':') {
                cout << boolalpha <<
                    client.sendText(line.substr(2)) << endl;
            } else {
                auto v = getVector(line);
                cout << boolalpha <<
                    client.sendData(v) << endl;
            }
        }
        return 0;
    } catch (const std::exception& e) {
        cerr << e.what() << endl;
        return EXIT_FAILURE;
    }
}

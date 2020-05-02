//
// Created by gw on 30.04.2020.
//
#include "enet_server.h"

#include <stdexcept>
#include <iostream>

using namespace std;

std::size_t EnetServer::g_counter = 0;

EnetServer::EnetServer(int port_num, std::size_t max_peer_number) {
    if (port_num <= 0) {
        throw invalid_argument("Bad port number: " + to_string(port_num));
    }
    address.host = ENET_HOST_ANY;
    address.port = port_num;
    server = enet_host_create(&address, max_peer_number, 2, 0, 0);
    if (server == nullptr) {
        throw logic_error("Cannot create server");
    }
    do_accept();
}

EnetServer::~EnetServer() {
    if (server != nullptr) {
        enet_host_destroy(server);
    }
}

void EnetServer::do_accept() {
    constexpr int TIME_OUT = 1000;
    ENetEvent event;
    while (true) {
        while (enet_host_service(server, &event, TIME_OUT) > 0) {
            switch (event.type) {
                case ENET_EVENT_TYPE_CONNECT:
                    printf("A new client connected from %x:%u.\n",
                           event.peer->address.host,
                           event.peer->address.port);
                    /* Store any relevant client information here. */
                    //event.peer->data = "";
                    break;
                case ENET_EVENT_TYPE_RECEIVE:
                    cout << "A packet of length " << event.packet->dataLength << " containing " << event.packet->data
                         << " was received from " << event.peer->data << " on channel " << event.channelID << '\n';
                    /* Clean up the packet now that we're done using it. */
                    enet_packet_destroy(event.packet);
                    break;
                case ENET_EVENT_TYPE_DISCONNECT:
                    cout << event.peer->data << "disconnected.\\n";
                    /* Reset the peer's client information. */
                    event.peer->data = nullptr;
                default:
                    cerr << "Unknown event.\n";
                    break;
            }
        }
    }
}

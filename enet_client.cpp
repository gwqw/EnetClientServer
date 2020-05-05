//
// Created by gw on 30.04.2020.
//
#include "enet_client.h"

#include <stdexcept>
#include <iostream>

using namespace std;

EnetClient::EnetClient() {
    client = enet_host_create(nullptr, 1, 2, 0, 0);
    if (client == nullptr) {
        throw runtime_error("Cannot create client");
    }
}

EnetClient::EnetClient(const std::string& host_name, int port)
    : EnetClient()
{
    connect(host_name, port);
}

bool EnetClient::connect(const string &host_name, int port) {
    if (port <= 0) {
        throw invalid_argument("Port number must be positive");
    }
    enet_address_set_host(&address, host_name.c_str());
    address.port = port;
    if (peer) enet_peer_disconnect(peer, 0);
    peer = enet_host_connect(client, &address, 2, 0);
    if (peer == nullptr) {
        return false;
    }

    ENetEvent event;
    constexpr int CONNECTION_TIMEOUT = 2000;
    if (enet_host_service(client, &event, CONNECTION_TIMEOUT) > 0 && event.type == ENET_EVENT_TYPE_CONNECT) {
        return true;
    } else {
        enet_peer_reset(peer);
        return false;
    }
}

void EnetClient::reconnect() {
    if (peer) enet_peer_disconnect(peer, 0);
    peer = enet_host_connect(client, &address, 2, 0);
    if (peer == nullptr) {
        return;
    }

    ENetEvent event;
    constexpr int CONNECTION_TIMEOUT = 2000;
    if (!(enet_host_service(client, &event, CONNECTION_TIMEOUT) > 0 && event.type == ENET_EVENT_TYPE_CONNECT)) {
        enet_peer_reset(peer);
    }
}

EnetClient::~EnetClient() {
    enet_peer_disconnect(peer, 0); // gently disconnection
    ENetEvent event;
    constexpr int DISCONNECTION_TIMEOUT = 2000;
    bool is_disconnected = false;
    while (enet_host_service(client, &event, DISCONNECTION_TIMEOUT) > 0)
    {
        if (is_disconnected) break;
        switch (event.type) {
            case ENET_EVENT_TYPE_RECEIVE:
                enet_packet_destroy(event.packet);
                break;
            case ENET_EVENT_TYPE_DISCONNECT:
                cerr << "Disconnection succeeded." << endl;
                is_disconnected = true;
                break;
        }
    }
    enet_host_destroy(client);
}

void EnetClient::sendText(const string &data) {
    constexpr int CHANNEL_ID = 0;
    ENetPacket* packet = enet_packet_create(
            data.c_str(),           // package
            data.size()+1,          // package size
            ENET_PACKET_FLAG_RELIABLE); // tcp
    enet_peer_send(peer, CHANNEL_ID, packet);
    enet_host_flush(client);
}

void EnetClient::sendData(const vector<int> &data) {
    constexpr int CHANNEL_ID = 1;
    ENetPacket* packet = enet_packet_create(
            data.data(),                // package
            data.size() * sizeof(int),  // package size
            ENET_PACKET_FLAG_UNRELIABLE_FRAGMENT); // udp
    enet_peer_send(peer, CHANNEL_ID, packet);
    enet_host_flush(client);
}




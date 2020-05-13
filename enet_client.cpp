#include "enet_client.h"

#include <stdexcept>
#include <iostream>
#include <thread>
#include <chrono>

#include "print_ip.h"

using namespace std;

EnetClient::EnetLibWrapper EnetClient::enetLibWrapper;

EnetClient::EnetLibWrapper::EnetLibWrapper() {
    if (enet_initialize() != 0) {
        cerr << "enet_initialize is failed" << endl;
        throw runtime_error("enet_initialize is failed");
    }
}

EnetClient::EnetLibWrapper::~EnetLibWrapper() {
    enet_deinitialize();
}


EnetClient::EnetClient() {
    client = enet_host_create(nullptr, 1, 2, 0, 0);
    if (client == nullptr) {
        throw runtime_error("Cannot create client");
    }
    thr = thread(&EnetClient::do_accept, this);
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

    constexpr int CONNECTION_TIMEOUT = 2000;
    this_thread::sleep_for(chrono::milliseconds(CONNECTION_TIMEOUT));
    if (is_connected) {
        return true;
    } else {
        enet_peer_reset(peer);
        peer = nullptr;
        return false;
    }
}

void EnetClient::reconnect() {
    peer = enet_host_connect(client, &address, 2, 0);
    if (peer == nullptr) {
        return;
    }

    constexpr int CONNECTION_TIMEOUT = 1000;
    this_thread::sleep_for(chrono::milliseconds(CONNECTION_TIMEOUT));
    if (!is_connected) {
        enet_peer_reset(peer);
        peer = nullptr;
    }
}

EnetClient::~EnetClient() {
    stop_flag = true;
    thr.join();
    if (peer) {
        enet_peer_disconnect(peer, 0); // gently disconnection
        ENetEvent event;
        constexpr int DISCONNECTION_TIMEOUT = 2000;
        bool is_disconnected = false;
        while (enet_host_service(client, &event, DISCONNECTION_TIMEOUT) > 0) {
            if (is_disconnected) break;
            switch (event.type) {
                case ENET_EVENT_TYPE_RECEIVE:
                    enet_packet_destroy(event.packet);
                    break;
                case ENET_EVENT_TYPE_DISCONNECT:
                    cerr << "Disconnection succeeded." << endl;
                    is_disconnected = true;
                    break;
                default:
                    break;
            }
        }
    }
    enet_host_destroy(client);
}

void EnetClient::do_accept() {
    ENetEvent event;
    while (!stop_flag) {
        while (enet_host_service(client, &event, accept_timeout_) > 0) {
            switch (event.type) {
                case ENET_EVENT_TYPE_CONNECT:
                    is_connected = true;
                    cerr << "A new client connected from " << event.peer->address.host
                         << ':' << event.peer->address.port << endl;
                    cerr << "Connected to server ";
                    print_ip(cerr, event.peer->address.host);
                    cerr << ':' << event.peer->address.port << endl;
                    /* Store any relevant client information here. */
                    //event.peer->data = "";
                    break;
                case ENET_EVENT_TYPE_RECEIVE:
                    cerr << "A packet of length " << event.packet->dataLength
                         << " was received from " << event.peer->data
                         << " on channel " << int(event.channelID) << endl;
                    /* Clean up the packet now that we're done using it. */
                    enet_packet_destroy(event.packet);
                    break;
                case ENET_EVENT_TYPE_DISCONNECT:
                    cerr << "disconnected." << endl;
                    /* Reset the peer's client information. */
                    is_connected = false;
                    event.peer->data = nullptr;
                    peer = nullptr;
                    break;
                default:
                    break;
            }
        }
    }
}

bool EnetClient::sendText(const string &data) {
    if (!is_connected) reconnect();
    if (!is_connected) return false;
    constexpr int CHANNEL_ID = 0;
    ENetPacket* packet = enet_packet_create(
            data.c_str(),           // package
            data.size()+1,          // package size
            ENET_PACKET_FLAG_RELIABLE); // tcp
    int err = enet_peer_send(peer, CHANNEL_ID, packet);
    return err == 0;
}

bool EnetClient::sendData(const vector<uint8_t> &data) {
    if (!is_connected) reconnect();
    if (!is_connected) return false;
    constexpr int CHANNEL_ID = 1;
    ENetPacket* packet = enet_packet_create(
            data.data(),                // package
            data.size(),                // package size
            0); // udp
    int err = enet_peer_send(peer, CHANNEL_ID, packet);
    return err == 0;
}







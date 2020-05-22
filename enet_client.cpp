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


EnetClient::EnetClient()
	: peer(nullptr), is_connected(false), stop_flag(false)
{
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
    if (peer.load()) enet_peer_disconnect(peer, 0);
    peer = enet_host_connect(client, &address, 2, 0);
    if (peer.load() == nullptr) {
        return false;
    }

    constexpr int CONNECTION_TIMEOUT = 2000;
    int attempts = 0;
    while (!is_connected.load() && attempts < 10) {
      this_thread::sleep_for(chrono::milliseconds(CONNECTION_TIMEOUT/ 10));
      ++attempts;
    }
    if (is_connected.load()) {
        return true;
    } else {
        enet_peer_reset(peer);
        peer = nullptr;
        return false;
    }
}

void EnetClient::reconnect() {
    peer = enet_host_connect(client, &address, 2, 0);
    if (peer.load() == nullptr) {
        return;
    }

    constexpr int CONNECTION_TIMEOUT = 1000;
    this_thread::sleep_for(chrono::milliseconds(CONNECTION_TIMEOUT));
    if (!is_connected.load()) {
        enet_peer_reset(peer);
        peer = nullptr;
    }
}

EnetClient::~EnetClient() {
    stop_flag.store(true);
    thr.join();
    if (peer.load()) {
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
    while (!stop_flag.load()) {
        while (enet_host_service(client, &event, accept_timeout_) > 0) {
            switch (event.type) {
                case ENET_EVENT_TYPE_CONNECT:
                    is_connected.store(true);
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
                    is_connected.store(false);
                    event.peer->data = nullptr;
                    peer.store(nullptr);
                    break;
                default:
                    break;
            }
        }
    }
}

bool EnetClient::sendRawData(const uint8_t *ptr, std::size_t size, EnetClient::ChannelType channel_type) {
  if (!is_connected.load()) reconnect();
  if (!is_connected.load()) return false;
  int flag = channel_type == RELIABLE_CHANNEL ? ENET_PACKET_FLAG_RELIABLE : 0;
  ENetPacket* packet = enet_packet_create(
          ptr,                // package
          size,               // package size
          flag);              // tcp or udp
  int err = enet_peer_send(peer, channel_type, packet);
  return err == 0;
}

bool EnetClient::sendText(const string &data) {
  return sendRawData(reinterpret_cast<const uint8_t*>(data.data()), data.size(), RELIABLE_CHANNEL);
}

bool EnetClient::sendData(const vector<uint8_t> &data) {
  return sendRawData(data.data(), data.size(), UNRELIABLE_CHANNEL);
}








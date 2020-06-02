#include "enet_client.h"

#include <stdexcept>
#include <iostream>
#include <thread>
#include <chrono>

#include "print_ip.h"

using namespace std;

EnetClient::EnetClient(TextCallbackFn text_func, DataCallbackFn data_func)
    : EnetPeer(move(text_func), move(data_func)),
	peer(nullptr), is_connected(false)
{
    client = enet_host_create(nullptr, 1, 2, 0, 0);
    if (client == nullptr) {
        throw runtime_error("Cannot create client");
    }
    thr = thread(&EnetClient::do_accept, this);
}

EnetClient::EnetClient(const std::string& host_name, int port,
                       TextCallbackFn text_func, DataCallbackFn data_func)
    : EnetClient(move(text_func), move(data_func))
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
    if (thr.joinable()) {
        thr.join();
    }
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
//        cerr << "A packet of length " << event.packet->dataLength
//             << " was received on channel " << int(event.channelID) << endl;
                    if (event.channelID == RELIABLE_CHANNEL && text_func) {
                        string data(reinterpret_cast<char*>(event.packet->data), event.packet->dataLength);
                        auto wrk_fnc = [this](string&& data) {
                            text_func(data);
                        };
                        string_thread_pool.addTask(wrk_fnc, move(data));
                    }
                    else if (event.channelID == UNRELIABLE_CHANNEL && data_func) {
                        std::vector<uint8_t> data(event.packet->data, event.packet->data + event.packet->dataLength);
                        auto wrk_fnc = [this](std::vector<uint8_t> data) {
                            data_func(data);
                        };
                        data_thread_pool.addTask(wrk_fnc, move(data));
                    }
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

bool EnetClient::sendRawData(const uint8_t *ptr, std::size_t size, ChannelType channel_type) {
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








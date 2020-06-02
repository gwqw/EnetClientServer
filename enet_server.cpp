#include "enet_server.h"

#include <stdexcept>
#include <iostream>
#include <utility>
#include <thread>

#include "print_ip.h"

using namespace std;

EnetServer::EnetServer(int port_num, std::size_t max_peer_number,
        TextCallbackFn text_func, DataCallbackFn data_func)
    : EnetPeer(move(text_func), move(data_func))
{
    start_listen(port_num, max_peer_number);
}

bool EnetServer::start_listen(int port_num, std::size_t max_peer_number) {
    if (is_started) return true;
    if (port_num <= 0) {
        return false;
    }
    address.host = ENET_HOST_ANY;
    address.port = port_num;
    server = enet_host_create(&address, max_peer_number, 2, 0, 0);
    if (server == nullptr) {
        return false;
    }
    thr = thread(&EnetServer::do_accept, this);
    is_started = true;
    return true;
}

EnetServer::~EnetServer() {
    stop();
}

void EnetServer::stop() {
    if (is_started) {
        disconnectAllPeers();
        stop_flag = true;
        if (thr.joinable()) {
            thr.join();
        }
        enet_host_destroy(server);
        is_started = false;
    }
}

void EnetServer::do_accept() {
    ENetEvent event;
    while (!stop_flag) {
        while (enet_host_service(server, &event, accept_timeout_) > 0) {
            switch (event.type) {
                case ENET_EVENT_TYPE_CONNECT:
                    cerr << "A new client connected from ";
                    print_ip(cerr, event.peer->address.host);
                    cerr << ':' << event.peer->address.port << endl;
                    /* Store any relevant client information here. */
                    //event.peer->data = "";
                    {
                        lock_guard<mutex> lk(mtx_);
                        peers.insert(event.peer);
                    }
                    break;
                case ENET_EVENT_TYPE_RECEIVE:
//                    cerr << "A packet of length " << event.packet->dataLength
//                         << " was received on channel " << int(event.channelID) << endl;
                    if (event.channelID == RELIABLE_CHANNEL && text_func) {
                        string data(reinterpret_cast<char*>(event.packet->data), event.packet->dataLength);
                        auto wrk_fnc = [this](string&& data){
                            text_func(data);
                        };
                        string_thread_pool.addTask(wrk_fnc, move(data));
                    } else if (event.channelID == UNRELIABLE_CHANNEL && data_func) {
                        vector<uint8_t> data(event.packet->data, event.packet->data + event.packet->dataLength);
                        auto wrk_fnc = [this](vector<uint8_t>&& data){
                            data_func(data);
                        };
                        data_thread_pool.addTask(wrk_fnc, move(data));
                    }
                    /* Clean up the packet now that we're done using it. */
                    enet_packet_destroy(event.packet);
                    break;
                case ENET_EVENT_TYPE_DISCONNECT:
                    cerr << "disconnected." << endl;
                    {
                        lock_guard<mutex> lk(mtx_);
                        peers.erase(event.peer);
                    }
                    /* Reset the peer's client information. */
                    event.peer->data = nullptr;
                    break;
                default:
                    break;
            }
        }
    }
}

void EnetServer::disconnectAllPeers() {
    //enet_impl->server->peers
    lock_guard<mutex> lk(mtx_);
    for (const auto peer : peers) {
        enet_peer_disconnect(peer, 0);
    }
}

bool EnetServer::sendRawData(const uint8_t *ptr, std::size_t size, ChannelType channel_type) {
    int flag = channel_type == RELIABLE_CHANNEL ? ENET_PACKET_FLAG_RELIABLE : 0;
    ENetPacket* packet = enet_packet_create(
            ptr,                // package
            size,               // package size
            flag);              // tcp or udp
    enet_host_broadcast(server, channel_type, packet);
    return true;
}

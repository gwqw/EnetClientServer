//
// Created by gw on 30.04.2020.
//
#include "enet_server.h"

#include <stdexcept>
#include <iostream>
#include <utility>
#include <cstring>

using namespace std;

EnetServer::EnetServer(int port_num, std::size_t max_peer_number)
    : EnetServer(port_num, max_peer_number, nullptr, nullptr)
{
}

EnetServer::EnetServer(int port_num, std::size_t max_peer_number,
        TextCallbackFn text_func, DataCallbackFn data_func)
    : text_func(move(text_func)), data_func(move(data_func)), string_thread_pool(1), data_thread_pool(1)
{
    if (port_num <= 0) {
        throw invalid_argument("Bad port number: " + to_string(port_num));
    }
    address.host = ENET_HOST_ANY;
    address.port = port_num;
    server = enet_host_create(&address, max_peer_number, 2, 0, 0);
    if (server == nullptr) {
        throw logic_error("Cannot create server");
    }
    thr = thread(&EnetServer::do_accept, this);
}

EnetServer::~EnetServer() {
    stop_flag = true;
    thr.join();
    if (server != nullptr) {
        enet_host_destroy(server);
    }
}

void EnetServer::do_accept() {
    constexpr int TIME_OUT = 1000;
    ENetEvent event;
    while (!stop_flag) {
        while (enet_host_service(server, &event, TIME_OUT) > 0) {
            switch (event.type) {
                case ENET_EVENT_TYPE_CONNECT:
                    cerr << "A new client connected from " << event.peer->address.host
                         << ':' << event.peer->address.port << endl;
                    /* Store any relevant client information here. */
                    //event.peer->data = "";
                    break;
                case ENET_EVENT_TYPE_RECEIVE:
                    cerr << "A packet of length " << event.packet->dataLength
                         << " was received from " << event.peer->data
                         << " on channel " << int(event.channelID) << endl;
                    if (event.channelID == RELIABLE_CHANNEL) {
                        string data(event.packet->dataLength, '\0');
                        copy(event.packet->data, event.packet->data + event.packet->dataLength, data.begin());
                        auto wrk_fnc = [this](string data){
                            text_func(data);
                        };
                        string_thread_pool.addTask(wrk_fnc, move(data));
                    } else { // UNRELIABLE_CHANNEL
                        vector<uint8_t> data(event.packet->dataLength);
                        memcpy((void*)data.data(), (void*)event.packet->data, event.packet->dataLength);
                        auto wrk_fnc = [this](vector<uint8_t> data){
                            data_func(data);
                        };
                        data_thread_pool.addTask(wrk_fnc, move(data));
                    }
                    /* Clean up the packet now that we're done using it. */
                    enet_packet_destroy(event.packet);
                    break;
                case ENET_EVENT_TYPE_DISCONNECT:
                    cout << "disconnected." << endl;
                    /* Reset the peer's client information. */
                    event.peer->data = nullptr;
                default:
                    break;
            }
        }
    }
}



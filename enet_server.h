/**
 * @file enet_server.h
 * @author gw
 * @brief Enet server realization, it can receive data by reliable channel and unreliable
*/
#pragma once
#ifndef ENET_TEST_ENET_SERVER_H
#define ENET_TEST_ENET_SERVER_H

#include <mutex>
#include <unordered_set>

#include "enet_peer.h"

/**
 * @brief C++ wrapper for enet-server from enet-library
 *
 * EnetServer can listen for incoming connections. It creates 2 channel: 0 with reliable data transfer (tcp),
 * 1 with unreliable data transfer (udp).
 * You can set call_back functions to process incoming data: text for reliable channel, data for unreliable one.
 * You can set accept timeout. Shorter timeout lets faster response for incoming event. But it loads processor harder
 */
class EnetServer : public EnetPeer {
public:
    // ctor && dtor
    static EnetServer& getInstance(int port_num, std::size_t max_peer_number,
                                  TextCallbackFn text_func, DataCallbackFn data_func) {
        static EnetServer server(port_num, max_peer_number, std::move(text_func), std::move(data_func));
        return server;
    }
    explicit EnetServer(int port_num, std::size_t max_peer_number = 10,
               TextCallbackFn text_func = nullptr, DataCallbackFn data_func = nullptr);
    EnetServer(const EnetServer&) = delete;
    EnetServer& operator=(const EnetServer&) = delete;
    ~EnetServer() override;
    bool start_listen(int port_num, std::size_t max_peer_number = 10);    /// starts server to listen incoming connections
    void stop();    ///< disconnects all peers and stops listening

    // senders
    bool sendRawData(const uint8_t* ptr, std::size_t size, ChannelType channel_type) override;
    // state
    bool isStarted() const {return is_started;}
private:
    void do_accept() override;
    void disconnectAllPeers();
private:
    ENetAddress address;
    ENetHost* server = nullptr;
    std::unordered_set<ENetPeer*> peers;
    std::mutex mtx_;
    bool is_started = false;
};

#endif //ENET_TEST_ENET_SERVER_H


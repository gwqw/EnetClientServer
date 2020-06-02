/**
 * @file enet_client.h
 * @author gw
 * @brief Enet client realization, it can send data using reliable channel and unreliable
*/
#pragma once
#ifndef ENET_TEST_ENET_CLIENT_H
#define ENET_TEST_ENET_CLIENT_H

#include <string>
#include <vector>
#include <atomic>
#include <memory>
#include <thread>

#include "enet/enet.h"

#include "enet_peer.h"

/**
 * @brief C++ wrapper for enet-client from enet-library
 *
 * EnetClient can connect to server with specified ip and port.
 * It creates 2 channel: 0 with reliable data transfer (tcp), 1 with unreliable data transfer (udp).
 */
class EnetClient : public EnetPeer {
public:
    explicit EnetClient(TextCallbackFn text_func = nullptr, DataCallbackFn data_func = nullptr);
    EnetClient(const std::string& host_name, int port,
               TextCallbackFn text_func = nullptr, DataCallbackFn data_func = nullptr); ///< creates client and connects to host_name:port
    EnetClient(const EnetClient&) = delete;
    EnetClient& operator=(const EnetClient&) = delete;
    ~EnetClient() override;

    bool connect(const std::string& host_name, int port); ///< connects to host_name:port
    void reconnect();                                     ///< tries to reconnect to set address and port
    bool isConnected() const {return is_connected;}       ///< checks connection state
    bool sendRawData(const uint8_t* ptr, std::size_t size, ChannelType channel_type) override;
private:
    void do_accept() override;
private:
    ENetHost *client = nullptr;
    ENetAddress address;
    std::atomic<ENetPeer*> peer;
    std::atomic<bool> is_connected;
};

#endif //ENET_TEST_ENET_CLIENT_H

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
#include <thread>

#include <enet/enet.h>

/**
 * @brief C++ wrapper for enet-client from enet-library
 *
 * EnetClient can connect to server with specified ip and port.
 * It creates 2 channel: 0 with reliable data transfer (tcp), 1 with unreliable data transfer (udp).
 */
class EnetClient {
    struct EnetLibWrapper {
        EnetLibWrapper();
        ~EnetLibWrapper();
    };
public:
    EnetClient();
    EnetClient(const std::string& host_name, int port); ///< creates client and connects to host_name:port
    EnetClient(const EnetClient&) = delete;
    EnetClient& operator=(const EnetClient&) = delete;
    ~EnetClient();

    bool connect(const std::string& host_name, int port); ///< connects to host_name:port
    void reconnect();                                     ///< tries to reconnect to set address and port
    bool isConnected() const {return is_connected;}       ///< checks connection state
    bool sendText(const std::string& data);               ///< send string using reliable channel, return true if data was sent
    bool sendData(const std::vector<uint8_t>& data);      ///< send raw data using unreliable channel

    void setAcceptTimeOut(std::uint32_t timeout) { accept_timeout_ = timeout;}
private:
    ENetHost *client = nullptr;
    ENetAddress address;
    std::atomic<ENetPeer*> peer{nullptr};
    std::atomic_bool is_connected{false};
    std::uint32_t accept_timeout_ = 200;
    std::atomic_bool stop_flag{false};
    std::thread thr;
    static EnetLibWrapper enetLibWrapper;

    void do_accept();
};

#endif //ENET_TEST_ENET_CLIENT_H

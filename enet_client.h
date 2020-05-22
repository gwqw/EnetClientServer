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
  enum ChannelType {
    RELIABLE_CHANNEL,    ///< reliable channel for text
    UNRELIABLE_CHANNEL,  ///< unreliable channel for data
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
    bool sendRawData(const uint8_t* ptr, std::size_t size, ChannelType channel_type = UNRELIABLE_CHANNEL);

    void setAcceptTimeOut(std::uint32_t timeout) { accept_timeout_ = timeout;}
private:
    ENetHost *client = nullptr;
    ENetAddress address;
    std::atomic<ENetPeer*> peer;
    std::atomic<bool> is_connected;
    std::uint32_t accept_timeout_ = 200;
    std::atomic<bool> stop_flag;
    std::thread thr;
    static EnetLibWrapper enetLibWrapper;

    void do_accept();
};

#endif //ENET_TEST_ENET_CLIENT_H

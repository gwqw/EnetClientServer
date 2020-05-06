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
    void reconnect();
    bool isConnected() const {return peer != nullptr;}
    bool sendText(const std::string& data);             ///< send string using reliable channel, return success
    bool sendData(const std::vector<int>& data);        ///< send raw data using unreliable channel
private:
    ENetHost *client = nullptr;
    ENetAddress address;
    std::atomic<ENetPeer*> peer{nullptr};
    std::atomic<bool> stop_flag{false};
    std::thread thr;
    static EnetLibWrapper enetLibWrapper;

    void do_accept();
};

#endif //ENET_TEST_ENET_CLIENT_H

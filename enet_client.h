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

#include <enet/enet.h>

class EnetClient {
public:
    EnetClient();
    EnetClient(const std::string& host_name, int port); ///< creates client and connects to host_name:port
    EnetClient(const EnetClient&) = delete;
    EnetClient& operator=(const EnetClient&) = delete;
    ~EnetClient();

    bool connect(const std::string& host_name, int port); ///< connects to host_name:port
    void reconnect();
    bool isConnected() const {return peer != nullptr;}
    void sendText(const std::string& data);             ///< send string using reliable channel
    void sendData(const std::vector<int>& data);        ///< send raw data using unreliable channel
private:
    ENetHost *client = nullptr;
    ENetAddress address;
    ENetPeer *peer = nullptr;
};

#endif //ENET_TEST_ENET_CLIENT_H

//
// Created by gw on 30.04.2020.
//
#pragma once
#ifndef ENET_TEST_ENET_CLIENT_H
#define ENET_TEST_ENET_CLIENT_H

#include <string>
#include <vector>

#include <enet/enet.h>

class EnetClient {
public:
    explicit EnetClient(const std::string& host_name, int port);
    EnetClient(const EnetClient&) = delete;
    EnetClient& operator=(const EnetClient&) = delete;
    ~EnetClient();

    void sendText(const std::string& data);
    void sendData(const std::vector<int>& data);
private:
    ENetHost *client = nullptr;
    ENetAddress address;
    ENetPeer *peer = nullptr;
};



#endif //ENET_TEST_ENET_CLIENT_H

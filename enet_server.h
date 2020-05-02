//
// Created by gw on 30.04.2020.
//
#pragma once
#ifndef ENET_TEST_ENET_SERVER_H
#define ENET_TEST_ENET_SERVER_H

#include <enet/enet.h>

class EnetServer {
public:
    // ctor && dtor
    explicit EnetServer(int port_num, std::size_t max_peer_number);
    EnetServer(const EnetServer&) = delete;
    EnetServer& operator=(const EnetServer&) = delete;
    ~EnetServer();

private:
    ENetAddress address;
    ENetHost* server = nullptr;
    static std::size_t g_counter;

    void do_accept();
};




#endif //ENET_TEST_ENET_SERVER_H

//
// Created by gw on 30.04.2020.
//
#pragma once
#ifndef ENET_TEST_ENET_SERVER_H
#define ENET_TEST_ENET_SERVER_H

#include <enet/enet.h>
#include <functional>
#include <string>
#include <vector>

enum ChannelType {
    TEXT_TYPE_CHANNEL,  // reliable channel for text
    DATA_TYPE_CHANNEL}; // unreliable channel for data

using TextCallbackFn = std::function<void(const std::string&)>;
using DataCallbackFn = std::function<void(const std::vector<int>&)>;

class EnetServer {
    struct Task {
        ChannelType channel;
        std::vector<uint8_t> data;
    };
public:
    // ctor && dtor
    explicit EnetServer(int port_num, std::size_t max_peer_number);
    EnetServer(int port_num, std::size_t max_peer_number,
            TextCallbackFn text_func, DataCallbackFn data_func);
    EnetServer(const EnetServer&) = delete;
    EnetServer& operator=(const EnetServer&) = delete;
    ~EnetServer();

    void setCallBackForText(TextCallbackFn func) {text_func = std::move(func);}
    void setCallBackForData(DataCallbackFn func) {data_func = std::move(func);}

private:
    ENetAddress address;
    ENetHost* server = nullptr;
    TextCallbackFn text_func;
    DataCallbackFn data_func;

    void do_accept();
};



#endif //ENET_TEST_ENET_SERVER_H


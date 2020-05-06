/**
 * @file enet_server.h
 * @author gw
 * @brief Enet server realization, it can receive data by reliable channel and unreliable
*/
#pragma once
#ifndef ENET_TEST_ENET_SERVER_H
#define ENET_TEST_ENET_SERVER_H

#include <enet/enet.h>
#include <functional>
#include <string>
#include <vector>
#include <atomic>
#include <thread>

#include "thread_pool.h"

enum ChannelType {
    RELIABLE_CHANNEL,    // reliable channel for text
    UNRELIABLE_CHANNEL}; // unreliable channel for data

using TextCallbackFn = std::function<void(const std::string&)>;
using DataCallbackFn = std::function<void(const std::vector<std::uint8_t>&)>;

class EnetServer {
    struct EnetLibWrapper {
        EnetLibWrapper();
        ~EnetLibWrapper();
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
    std::atomic<bool> stop_flag{false};
    std::thread thr;

    ThreadPool string_thread_pool;
    ThreadPool data_thread_pool;

    static EnetLibWrapper enetLibWrapper;

    void do_accept();
};

#endif //ENET_TEST_ENET_SERVER_H


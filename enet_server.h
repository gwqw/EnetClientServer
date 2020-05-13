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
    RELIABLE_CHANNEL,    ///< reliable channel for text
    UNRELIABLE_CHANNEL}; ///< unreliable channel for data

using TextCallbackFn = std::function<void(const std::string&)>;
using DataCallbackFn = std::function<void(const std::vector<std::uint8_t>&)>;

/**
 * @brief C++ wrapper for enet-server from enet-library
 *
 * EnetServer can listen for incoming connections. It creates 2 channel: 0 with reliable data transfer (tcp),
 * 1 with unreliable data transfer (udp).
 * You can set call_back functions to process incoming data: text for reliable channel, data for unreliable one.
 * You can set accept timeout. Shorter timeout lets faster response for incoming event. But it loads processor harder
 */
class EnetServer {
    struct EnetLibWrapper {
        EnetLibWrapper();
        ~EnetLibWrapper();
    };
public:
    // ctor && dtor
    static EnetServer& getInstance(int port_num, std::size_t max_peer_number,
                                  TextCallbackFn text_func, DataCallbackFn data_func) {
        static EnetServer server(port_num, max_peer_number, std::move(text_func), std::move(data_func));
        return server;
    }
    explicit EnetServer(int port_num, std::size_t max_peer_number);
    EnetServer(int port_num, std::size_t max_peer_number,
            TextCallbackFn text_func, DataCallbackFn data_func);
    EnetServer(const EnetServer&) = delete;
    EnetServer& operator=(const EnetServer&) = delete;
    ~EnetServer();

    // setters
    void setCallBackForText(TextCallbackFn func) {text_func = std::move(func);} ///< callback for reliable data (text)
    void setCallBackForData(DataCallbackFn func) {data_func = std::move(func);} ///< callback for unreliable data
    void setAcceptTimeOut(std::uint32_t timeout) {accept_timeout_ = timeout;}

private:
    ENetAddress address;
    ENetHost* server = nullptr;
    TextCallbackFn text_func;
    DataCallbackFn data_func;
    std::uint32_t accept_timeout_ = 100;

    std::atomic<bool> stop_flag{false};
    std::thread thr;
    ThreadPool string_thread_pool;
    ThreadPool data_thread_pool;

    static EnetLibWrapper enetLibWrapper;

    void do_accept();
};

#endif //ENET_TEST_ENET_SERVER_H


#pragma once
#ifndef ENET_TEST_ENET_PEER_H
#define ENET_TEST_ENET_PEER_H

#include <functional>
#include <string>
#include <vector>
#include <atomic>

#include <enet/enet.h>

#include "thread_pool.h"

enum ChannelType {
    RELIABLE_CHANNEL,    ///< reliable channel for text
    UNRELIABLE_CHANNEL,  ///< unreliable channel for data
};

using TextCallbackFn = std::function<void(const std::string&)>;
using DataCallbackFn = std::function<void(const std::vector<std::uint8_t>&)>;

class EnetPeer {
    struct EnetLibWrapper {
        EnetLibWrapper();

        ~EnetLibWrapper();
    };
public:
    EnetPeer();
    EnetPeer(TextCallbackFn text_func, DataCallbackFn data_func);
    virtual ~EnetPeer() = default;
    // senders
    bool sendText(const std::string& data);               ///< send string using reliable channel, return true if data was sent
    bool sendData(const std::vector<uint8_t>& data);      ///< send raw data using unreliable channel
    virtual bool sendRawData(const uint8_t* ptr, std::size_t size, ChannelType channel_type) = 0;
    // setters
    void setCallBackForText(TextCallbackFn func) { text_func = std::move(func); } ///< callback for reliable data (text)
    void setCallBackForData(DataCallbackFn func) { data_func = std::move(func); } ///< callback for unreliable data
    void setAcceptTimeOut(std::uint32_t timeout) { accept_timeout_ = timeout; }
protected:
    virtual void do_accept() = 0;
protected:
    std::uint32_t accept_timeout_ = 200;
    std::atomic<bool> stop_flag;
    std::thread thr;
    TextCallbackFn text_func = nullptr;
    DataCallbackFn data_func = nullptr;
    ThreadPool string_thread_pool;
    ThreadPool data_thread_pool;
private:
    static EnetLibWrapper enetLibWrapper;
};


#endif //ENET_TEST_ENET_PEER_H

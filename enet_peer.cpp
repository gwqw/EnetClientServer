#include "enet_peer.h"

#include <iostream>
#include <utility>

using namespace std;

EnetPeer::EnetLibWrapper EnetPeer::enetLibWrapper;

// EnetLibWrapper
EnetPeer::EnetLibWrapper::EnetLibWrapper() {
    if (enet_initialize() != 0) {
        cerr << "enet_initialize is failed" << endl;
        throw runtime_error("enet_initialize is failed");
    }
}

EnetPeer::EnetLibWrapper::~EnetLibWrapper() {
    enet_deinitialize();
}

// EnetPeer
EnetPeer::EnetPeer()
        : stop_flag(false),
          string_thread_pool(1), data_thread_pool(1)
{}

EnetPeer::EnetPeer(TextCallbackFn text_func, DataCallbackFn data_func)
        : stop_flag(false),
          string_thread_pool(1), data_thread_pool(1),
          text_func(move(text_func)), data_func(move(data_func))
{}

bool EnetPeer::sendText(const string &data) {
    return sendRawData(reinterpret_cast<const uint8_t*>(data.data()), data.size(), RELIABLE_CHANNEL);
}

bool EnetPeer::sendData(const vector<uint8_t> &data) {
    return sendRawData(data.data(), data.size(), UNRELIABLE_CHANNEL);
}
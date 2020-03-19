#include "archer/net/poller/epoll_poller.hpp"

using namespace archer;

Poller::~Poller() {
    for(auto channel :channels_) {
        delete channel.second;
    }
}
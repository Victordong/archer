#ifndef _ARCHER_EPOLL_POLLER_HPP
#define _ARCHER_EPOLL_POLLER_HPP

#pragma once
#include <vector>
#include "archer/net/poller/poller.hpp"

namespace archer {

class EpollPoller : Poller {
   public:
    EpollPoller(Eventloop&);
    ~EpollPoller();

    virtual void AddChannel(Channel&) override;

    virtual void UpdateChannel(Channel&) override;

    virtual void RemoveChannel(Channel&) override;

    virtual void Poll(int timeoutMs, ChannelList& activeChannels) override;

   private:
    static const int kInitSize_ = 16;

    void update(int, Channel&);

    int epfd() { return epfd_.get()->fd(); };

    void fillActiveChannels(int numEvents, ChannelList& activeChannels) const;

    using EventList = std::vector<epoll_event>;

    EventList events_;
    SocketPtr epfd_;
};
};  // namespace archer

#endif  // _ARCHER_EPOLL_POLLER_HPP

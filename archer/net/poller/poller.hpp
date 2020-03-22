#ifndef _ARCHER_POLLER_HPP
#define _ARCHER_POLLER_HPP

#include <map>
#include <memory>
#include <mutex>
#include "archer/net/eventloop/channel.hpp"

namespace archer {

class Poller : noncopyable {
   public:
    using ChannelList = std::vector<Channel*>;

    Poller(Eventloop& loop) : loop_(loop){};

    ~Poller();

    virtual void AddChannel(Channel&) = 0;

    virtual void UpdateChannel(Channel&) = 0;

    virtual void RemoveChannel(Channel&) = 0;

    virtual void Poll(int timeoutMs, ChannelList& activeChannels) = 0;

   protected:
    using ChannelMap = std::map<int, Channel*>;
    ChannelMap channels_;

   private:
    Eventloop& loop_;
};
};  // namespace archer

#endif  // _ARCHER_POLLER_HPP

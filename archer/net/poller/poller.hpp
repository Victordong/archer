#ifndef _ARCHER_POLLER_HPP
#define _ARCHER_POLLER_HPP

#include <map>
#include <memory>
#include <mutex>

#include "archer/net/eventloop/channel.hpp"

namespace archer {

class PollerImp : noncopyable {
   public:
    using ChannelList = std::vector<Channel*>;

    PollerImp(){};

    virtual void AddChannel(Channel&) = 0;

    virtual void UpdateChannel(Channel&) = 0;

    virtual void RemoveChannel(Channel&) = 0;

    virtual void Poll(int timeoutMs, ChannelList& activeChannels) = 0;

   protected:
    using ChannelMap = std::map<int, Channel*>;
    ChannelMap channels_;
};

class Poller : noncopyable {
   public:
    using ChannelList = std::vector<Channel*>;

    Poller();

    ~Poller();

    void AddChannel(Channel& ch) { imp_->AddChannel(ch); };

    void UpdateChannel(Channel& ch) { imp_->UpdateChannel(ch); };

    void RemoveChannel(Channel& ch) { imp_->RemoveChannel(ch); };

    void Poll(int timeoutMs, ChannelList& activeChannels) {
        imp_->Poll(timeoutMs, activeChannels);
    };

   private:
    std::unique_ptr<PollerImp> imp_;
};

};  // namespace archer

#endif  // _ARCHER_POLLER_HPP

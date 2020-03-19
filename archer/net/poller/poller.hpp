#include <map>
#include <memory>
#include <mutex>
#include "archer/net/eventloop/channel.hpp"

namespace archer {

class Poller : noncopyable {
   public:
    using ChannelList = std::vector<Channel*>;

    virtual void AddChannel(Channel&) = 0;

    virtual void UpdateChannel(Channel&) = 0;

    virtual void RemoveChannel(Channel&) = 0;

    virtual int poll(int timeoutMs, ChannelList& activeChannels) = 0;

    Poller(Eventloop& loop) : loop_(loop){};

    ~Poller();

   protected:
    using ChannelMap = std::map<int, Channel*>;
    ChannelMap channels_;

   private:
    Eventloop& loop_;
};
};  // namespace archer

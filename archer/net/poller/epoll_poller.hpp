#include "archer/net/poller/poller.hpp"
#include <vector>

namespace archer {

class EpollPoller : Poller {
   public:
    EpollPoller(Eventloop&);
    ~EpollPoller();

    virtual void AddChannel(Channel&) override;

    virtual void UpdateChannel(Channel&) override;

    virtual void RemoveChannel(Channel&) override;

    virtual int poll(int timeoutMs, ChannelList& activeChannels) override;

   private:
    static const int kInitSize_ = 16;

    void update(int, Channel&);

    void fillActiveChannels(int numEvents, ChannelList& activeChannels) const;

    using EventList = std::vector<epoll_event>;

    EventList events_;
    int epfd_;
};
};  // namespace archer

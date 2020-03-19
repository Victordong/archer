#include <sys/epoll.h>
#include <atomic>
#include <boost/scoped_ptr.hpp>
#include <iostream>
#include <memory>
#include <vector>
#include "archer/base/noncopyable.hpp"

namespace archer {

static const int kReadEvent = EPOLLIN | EPOLLPRI | EPOLLHUP;
static const int kWriteEvent = EPOLLOUT;
static const int kErrorEvent = EPOLLERR;
static const int kNoneEvent = 0;

class EpollPoller;

class Channel;

class Eventloop final : noncopyable {
   public:
    Eventloop();
    ~Eventloop();

    void RemoveChannel(Channel&);
    void UpdateChannel(Channel&);
    void AddChannel(Channel&);

    void loop();

    void quit() { quit_ = true; };

   private:
    using ChannelList = std::vector<Channel*>;

    static const int kPollTimeMs = 1000 * 10;

    std::atomic_bool quit_;
    std::atomic_bool looping_;

    boost::scoped_ptr<EpollPoller> poller_;
    ChannelList active_channels_;
};
};  // namespace archer

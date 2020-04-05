#ifndef _ARCHER_EVENTLOOP_HPP
#define _ARCHER_EVENTLOOP_HPP

#include <assert.h>
#include <sys/epoll.h>
#include <sys/timerfd.h>
#include <unistd.h>
#include <atomic>
#include <iostream>
#include <memory>
#include <mutex>
#include <map>
#include <vector>
#include "archer/base/noncopyable.hpp"
#include "archer/net/type.hpp"

namespace archer {

static const int kReadEvent = EPOLLIN | EPOLLPRI | EPOLLHUP;
static const int kWriteEvent = EPOLLOUT;
static const int kErrorEvent = EPOLLERR;
static const int kNoneEvent = 0;
static const int kBufferSize = 65536;

class Poller;

class Channel;

class TimerQueue;

class Timer;

using TimerPtr = std::shared_ptr<Timer>;

using TimerId = std::shared_ptr<Timer>;

using ChannelPtr = std::shared_ptr<Channel>;

class Eventloop final : noncopyable {
   public:
    Eventloop();
    ~Eventloop();

    void RemoveChannel(Channel&);
    void UpdateChannel(Channel&);
    void AddChannel(Channel&);

    void Loop();

    void quit();

    TimerId RunAt(const TimerCallback& cb, const Timestamp& time);
    TimerId RunAfter(const TimerCallback& cb, int delay);
    TimerId RunEvery(const TimerCallback& cb, double interval);
    void CancelTimer(TimerId& ti);

    void RunInLoop(const Functor& func);
    void QueueInLoop(const Functor& func);

    char* buffer() { return buffer_; };

   private:
    using ChannelList = std::vector<Channel*>;

    void HandleRead();
    void DoPendingFunctors();

    int wakeup_fd() const { return wakeup_fd_.fd(); };

    void wakeup();

    static const int kPollTimeMs = 1000 * 10;

    std::atomic_bool quit_;
    std::atomic_bool looping_;
    std::atomic_bool do_pending_;

    std::shared_ptr<Poller> poller_;
    ChannelList active_channels_;

    Socket wakeup_fd_;
    std::unique_ptr<Channel> wakeup_channel_;

    std::unique_ptr<TimerQueue> timer_queue_;
    std::vector<Functor> pending_functors_;

    char buffer_[kBufferSize];

    std::mutex mutex_;
};
};  // namespace archer

#endif  // _ARCHER_EVENTLOOP_HPP

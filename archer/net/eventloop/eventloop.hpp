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
#include <vector>
#include "archer/base/noncopyable.hpp"
#include "archer/base/thread/current_thread.hpp"
#include "archer/net/type.hpp"

namespace archer {

static const int kReadEvent = EPOLLIN | EPOLLPRI | EPOLLHUP;
static const int kWriteEvent = EPOLLOUT;
static const int kErrorEvent = EPOLLERR;
static const int kNoneEvent = 0;

class EpollPoller;

class Channel;

class TimerQueue;

class Timer;

using TimerPtr = std::shared_ptr<Timer>;

using TimerId = std::weak_ptr<Timer>;

class Eventloop final : noncopyable {
   public:
    Eventloop();
    ~Eventloop();

    void RemoveChannel(Channel&);
    void UpdateChannel(Channel&);
    void AddChannel(Channel&);

    void Loop();

    void quit();

    TimerId RunAt(const Timestamp& time, const TimerCallback& cb);
    TimerId RunAfter(int delay, const TimerCallback& cb);
    TimerId RunEvery(double interval, const TimerCallback& cb);
    void CancelTimer(TimerId& ti);

    void RunInLoop(const Functor& func);
    void QueueInLoop(const Functor& func);

    long long tid() { return tid_; };

   private:
    using ChannelList = std::vector<Channel*>;

    void HandleRead();
    void DoPendingFunctors();

    int wakeup_fd() { return wakeup_fd_.get()->fd(); };

    void wakeup();

    static const int kPollTimeMs = 1000 * 10;

    std::atomic_bool quit_;
    std::atomic_bool looping_;
    std::atomic_bool do_pending_;

    SocketPtr wakeup_fd_;
    std::unique_ptr<Channel> wakeup_channel_;

    std::unique_ptr<EpollPoller> poller_;
    ChannelList active_channels_;

    std::unique_ptr<TimerQueue> timer_queue_;
    std::vector<Functor> pending_functors_;

    std::mutex mutex_;

    const pid_t tid_;
};
};  // namespace archer

#endif  // _ARCHER_EVENTLOOP_HPP

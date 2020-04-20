#ifndef _ARCHER_EVENTLOOP_HPP
#define _ARCHER_EVENTLOOP_HPP

#include "archer/net/archer_imp.hpp"

#include "archer/base/noncopyable.hpp"

namespace archer {

static const int kReadEvent = EPOLLIN | EPOLLPRI | EPOLLHUP;
static const int kWriteEvent = EPOLLOUT;
static const int kErrorEvent = EPOLLERR;
static const int kNoneEvent = 0;

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
    TimerId RunAfter(const TimerCallback& cb, timestamp_t delay);
    TimerId RunEvery(const TimerCallback& cb, timestamp_t interval);
    void CancelTimer(TimerId& ti);

    void RunInLoop(const Functor& func);
    void QueueInLoop(const Functor& func);

    void callIdles();
    IdleId RegisterIdle(int idle,const TcpConnPtr & ptr,const TcpCallback& cb);
    void UnRegisterIdle(const IdleId&);
    void UpdateIdle(const IdleId&);

    std::atomic_uint64_t& total() { return total_; };

   private:
    using ChannelList = std::vector<Channel*>;

    void handleRead();
    void doPendingFunctors();

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

    std::mutex mutex_;

    std::atomic_uint64_t total_;

    int thread_id_;

    bool idle_enabled_;

    IdleMap idle_map_;

    TimerId idle_timerid_;
};
};  // namespace archer

#endif  // _ARCHER_EVENTLOOP_HPP

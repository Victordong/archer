#ifndef _ARCHER_TIMER_QUEUE
#define _ARCHER_TIMER_QUEUE

#include <sys/time.h>
#include <unistd.h>

#include <functional>
#include <map>

#include "archer/net/eventloop/channel.hpp"

namespace archer {

class Timer;

class TimerQueue : noncopyable {
   public:
    using Entry = std::pair<Timestamp, Timer*>;

    using TimerMap = std::map<Entry, TimerPtr, std::less<Entry>>;

    using TimerList = std::vector<Entry>;

    TimerQueue(Eventloop* loop);
    ~TimerQueue();

    TimerId AddTimer(const TimerCallback& cb,
                     const Timestamp& when,
                     int interval = 0);

    void AddTimerInLoop(const TimerPtr& timer);

    void CancelTimer(const TimerId& ti);

    void CancelTimerInLoop(const TimerId& ti);

   private:
    void handleRead();
    void GetExpired(Timestamp now);
    void Reset(const std::vector<Entry>& expired, Timestamp now);

    bool Insert(const TimerPtr& timer);
    void ResetTimerfd(int timestamp);

    int timerfd() { return timerfd_.fd(); };

    Eventloop* loop_;
    Socket timerfd_;
    Channel timerfd_channel_;
    TimerMap timers_;

    std::vector<TimerId> expire_timers_;
};
};  // namespace archer

#endif  // _ARCHER_TIMER_QUEUE

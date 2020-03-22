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
    using Entry = std::pair<Timestamp, unsigned int>;

    struct TimerCompare {
        bool operator()(const Entry& lhs, const Entry& rhs) const {
            return lhs.first < rhs.first;
        };
    };

    using TimerList = std::map<Entry, std::shared_ptr<Timer>, TimerCompare>;

    TimerQueue(Eventloop& loop);
    ~TimerQueue();

    TimerId AddTimer(const TimerCallback& cb,
                     const Timestamp& when,
                     int interval = 0);

    void AddTimerInLoop(const TimerPtr& timer);

    void CancelTimer(const TimerId& ti);

    

   private:
    void HandleRead();
    void GetExpired(Timestamp now);
    void Reset(const std::vector<Entry>& expired, Timestamp now);

    bool Insert(const TimerPtr& timer);
    void ResetTimerfd(int timestamp);

    int timerfd() { return timerfd_.get()->fd(); };

    Eventloop& loop_;
    SocketPtr timerfd_;
    Channel timerfd_channel_;
    TimerList timers_;

    std::vector<TimerId> expire_timers_;
};
};  // namespace archer

#endif  // _ARCHER_TIMER_QUEUE

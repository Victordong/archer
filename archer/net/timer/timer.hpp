#ifndef _ARCHER_HPP
#define _ARCHER_HPP

#include "archer/net/timer/timer_queue.hpp"

namespace archer {

class Timer : noncopyable {
    friend class TimerQueue;

   public:
    enum TimerStatus {
        Finished,
        Canceled,
        Pending,
        Error,
    };

    Timer(const TimerCallback& cb, const Timestamp& when, int interval);

    int expiration();

    int interval() { return interval_; };

    Timestamp when() { return when_; };

    TimerStatus status() { return status_; };

    unsigned int total_num() { return total_num_; };

   private:
    void set_status(TimerStatus status) { status_ = status; };

    Timestamp when_;
    TimerCallback timer_callback_;
    int interval_;
    bool repeat_;
    static unsigned int total_num_;
    TimerStatus status_;
};

};  // namespace archer

#endif  // _ARCHER_HPP

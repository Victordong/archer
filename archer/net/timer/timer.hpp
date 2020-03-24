#ifndef _ARCHER_HPP
#define _ARCHER_HPP

#include "archer/net/timer/timer_queue.hpp"

namespace archer {

class Timer : noncopyable {
    friend class TimerQueue;

   public:
    enum TimerStatus {
        Finished,
        Running,
        Canceled,
        Pending,
        Error,
    };

    Timer(const TimerCallback& cb, const Timestamp& when, int interval, TimerStatus status=Pending);

    int expiration();

    int interval() { return interval_; };

    Timestamp when() { return when_; };

    void set_when(Timestamp when) { when_ = when; };

    TimerStatus status() { return status_; };

    unsigned int total_num() { return total_num_; };

    void timer_callback() { timer_callback_(); };

    bool repeat() { return repeat_; };

   private:
    static unsigned int total_num_;

    void set_status(TimerStatus status) { status_ = status; };

    Timestamp when_;
    TimerCallback timer_callback_;

    int interval_;
    bool repeat_;

    TimerStatus status_;
};

};  // namespace archer

#endif  // _ARCHER_HPP

#ifndef _ARCHER_HPP
#define _ARCHER_HPP

#include "archer/net/timer/timer_queue.hpp"

namespace archer {

class Timer : noncopyable {

   public:
    enum TimerStatus {
        Finished,
        Running,
        Canceled,
        Pending,
        Error,
    };

    Timer(const TimerCallback& cb,
          const Timestamp& when,
          timestamp_t interval,
          TimerStatus status = Pending);

    timestamp_t expiration();

    timestamp_t interval() { return interval_; };

    Timestamp when() { return when_; };

    void set_when(Timestamp when) { when_ = when; };

    TimerStatus status() { return status_; };

    unsigned int total_num() { return total_num_; };

    void timer_callback() { timer_callback_(); };

    bool repeat() { return repeat_; };

    void set_status(TimerStatus status) { status_ = status; };

   private:
    static unsigned int total_num_;

    Timestamp when_;
    TimerCallback timer_callback_;

    timestamp_t interval_;
    bool repeat_;

    TimerStatus status_;
};

};  // namespace archer

#endif  // _ARCHER_HPP

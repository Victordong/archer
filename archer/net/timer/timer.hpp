#ifndef _ARCHER_HPP
#define _ARCHER_HPP

#include "archer/net/timer/timer_queue.hpp"

namespace archer {

class Timer : noncopyable {
   public:

    Timer(const TimerCallback& cb, const Timestamp& when, int interval);

    int expiration();

    int interval() { return interval_; };

    Timestamp when() { return when_; };

    unsigned int total_num() { return total_num_; };

   private:
    Timestamp when_;
    TimerCallback timer_callback_;
    int interval_;
    bool repeat_;
    static unsigned int total_num_;
    static int status;
};

};  // namespace archer

#endif  // _ARCHER_HPP

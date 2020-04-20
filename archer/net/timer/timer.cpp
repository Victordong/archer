#include "archer/net/timer/timer.hpp"

using namespace archer;

unsigned int Timer::total_num_ = 0;

Timer::Timer(const TimerCallback& cb,
             const Timestamp& when,
             timestamp_t interval,
             TimerStatus status)
    : timer_callback_(cb),
      when_(when),
      interval_(interval),
      status_(status),
      repeat_(interval > 0) {}

timestamp_t Timer::expiration() {
    return when_.expiration();
}

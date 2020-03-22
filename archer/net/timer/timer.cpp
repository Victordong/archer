#include "archer/net/timer/timer.hpp"

using namespace archer;

unsigned int Timer::total_num_ = 0;

Timer::Timer(const TimerCallback& cb, const Timestamp& when, int interval)
    : timer_callback_(cb), when_(when), interval_(interval) {}

int Timer::expiration() {
    return when_.expiration();
}

#include "archer/net/timer/timer_queue.hpp"
#include <string.h>
#include "archer/net/timer/timer.hpp"

using namespace archer;

TimerQueue::TimerQueue(Eventloop& loop)
    : loop_(loop),
      timerfd_(new Socket(
          timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC))),
      timerfd_channel_(loop, timerfd()) {
    assert(timerfd() > 0);
    timerfd_channel_.set_read_callback([=]() { this->HandleRead(); });
    timerfd_channel_.EnableReading();
    loop_.AddChannel(timerfd_channel_);
}

TimerId TimerQueue::AddTimer(const TimerCallback& cb,
                             const Timestamp& ts,
                             int interval) {
    TimerPtr timer_ptr(new Timer(cb, ts, interval));

    loop_.RunInLoop([&]() { this->AddTimerInLoop(timer_ptr); });

    return std::weak_ptr<Timer>(timer_ptr);
}

void TimerQueue::AddTimerInLoop(const TimerPtr& timer) {
    assert(loop_.tid() == CurrentThread::tid());

    bool earlier = Insert(timer);
    if (earlier) {
        ResetTimerfd(timer->expiration());
    }
}

bool TimerQueue::Insert(const TimerPtr& timer) {
    assert(loop_.tid() == CurrentThread::tid());

    timers_.insert(std::pair<Entry, TimerPtr>(
        Entry(timer->when(), timer->total_num()), timer));

    TimerList::const_iterator iter = timers_.begin();
    if (iter->first.first==timer->when()){
        return true;
    } else {
        return false;
    }
}

void TimerQueue::GetExpired(Timestamp now) {
    expire_timers_.clear();

}

void TimerQueue::ResetTimerfd(int timestamp) {
    assert(loop_.tid() == CurrentThread::tid());

    itimerspec howlong;
    ::bzero(&howlong, sizeof howlong);
    howlong.it_value.tv_sec = timestamp / Timestamp::kMicroSecondsPerSecond;
    howlong.it_value.tv_nsec =
        (timestamp % Timestamp::kMicroSecondsPerSecond) * Timestamp::kNanoSecondsPerMicroSecond;
    int result = timerfd_settime(timerfd(), 0, &howlong, nullptr);
    assert(result != -1);
}

void TimerQueue::CancelTimer(const TimerId& ti) {}
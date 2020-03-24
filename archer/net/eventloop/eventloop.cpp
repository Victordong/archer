#include "archer/net/eventloop/eventloop.hpp"
#include <sys/eventfd.h>
#include "archer/net/poller/epoll_poller.hpp"
#include "archer/net/timer/timer_queue.hpp"

using namespace archer;

Eventloop::Eventloop()
    : quit_(true),
      looping_(false),
      do_pending_(false),
      poller_(new Poller()),
      timer_queue_(new TimerQueue(this)),
      wakeup_fd_(new Socket(eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC))),
      wakeup_channel_(new Channel(this, wakeup_fd())) {
    assert(wakeup_fd() > 0);
    poller_->AddChannel(*wakeup_channel_);
    wakeup_channel_->set_read_callback([&]() {
        uint64_t buf;
        ::read(wakeup_channel_->fd(), &buf, sizeof buf);
    });
    wakeup_channel_->EnableReading();
}

Eventloop::~Eventloop() {}

void Eventloop::Loop() {
    quit_ = false;
    looping_ = true;

    while (!quit_) {
        poller_->Poll(kPollTimeMs, active_channels_);
        for (auto channel : active_channels_) {
            channel->HandleEvent();
        }
        active_channels_.clear();
        DoPendingFunctors();
    }
}

void Eventloop::quit() {
    quit_ = true;

    wakeup();
}

void Eventloop::HandleRead() {}

void Eventloop::wakeup() {
    uint64_t buf = 1;
    ::write(wakeup_fd(), &buf, sizeof buf);
}

void Eventloop::AddChannel(Channel& channel) {
    poller_->AddChannel(channel);
}

void Eventloop::RemoveChannel(Channel& channel) {
    poller_->RemoveChannel(channel);
}

void Eventloop::UpdateChannel(Channel& channel) {
    poller_->UpdateChannel(channel);
}

void Eventloop::RunInLoop(const Functor& func) {
    QueueInLoop(func);
}

void Eventloop::QueueInLoop(const Functor& func) {
    {
        std::lock_guard<std::mutex> gurad(mutex_);
        pending_functors_.push_back(func);
    }

    wakeup();
}

void Eventloop::DoPendingFunctors() {
    std::vector<Functor> functors;
    do_pending_ = true;

    {
        std::lock_guard<std::mutex> gurad(mutex_);
        functors.swap(pending_functors_);
    }

    for (auto func : functors) {
        func();
    }

    do_pending_ = false;
}

TimerId Eventloop::RunAt(const Timestamp& time, const TimerCallback& cb) {
    return timer_queue_->AddTimer(cb, time);
}

TimerId Eventloop::RunAfter(int delay, const TimerCallback& cb) {
    return timer_queue_->AddTimer(cb, Timestamp::Now() + delay);
}

TimerId Eventloop::RunEvery(double interval, const TimerCallback& cb) {
    return timer_queue_->AddTimer(cb, Timestamp::Now() + interval, interval);
}

void Eventloop::CancelTimer(TimerId& ti) {
    timer_queue_->CancelTimer(ti);
}

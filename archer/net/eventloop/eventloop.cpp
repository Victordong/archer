#include "archer/net/eventloop/eventloop.hpp"

#include <sys/eventfd.h>

#include "archer/base/thread.hpp"
#include "archer/net/poller/epoll_poller.hpp"
#include "archer/net/timer/idle.hpp"
#include "archer/net/timer/timer_queue.hpp"

using namespace archer;

Eventloop::Eventloop()
    : quit_(true),
      looping_(false),
      do_pending_(false),
      poller_(new Poller()),
      timer_queue_(new TimerQueue(this)),
      wakeup_fd_(eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC)),
      wakeup_channel_(new Channel(this, wakeup_fd())),
      idle_enabled_(false) {
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
    thread_id_ = CurrentThread::gettid();

    while (!quit_) {
        poller_->Poll(kPollTimeMs, active_channels_);
        for (auto channel : active_channels_) {
            channel->handleEvent();
        }
        active_channels_.clear();
        doPendingFunctors();
    }
}

void Eventloop::quit() {
    quit_ = true;

    wakeup();
}

void Eventloop::handleRead() {}

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

void Eventloop::doPendingFunctors() {
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

TimerId Eventloop::RunAt(const TimerCallback& cb, const Timestamp& time) {
    return timer_queue_->AddTimer(cb, time);
}

TimerId Eventloop::RunAfter(const TimerCallback& cb, int delay) {
    return timer_queue_->AddTimer(cb, Timestamp::Now() + delay);
}

TimerId Eventloop::RunEvery(const TimerCallback& cb, double interval) {
    return timer_queue_->AddTimer(cb, Timestamp::Now() + interval, interval);
}

void Eventloop::CancelTimer(TimerId& ti) {
    timer_queue_->CancelTimer(ti);
}

void Eventloop::callIdles() {
    auto current = Timestamp::Now();
    for (auto& item : idle_map_) {
        auto idle_time = item.first*Timestamp::kMicroSecondsPerSecond;
        auto& lst = item.second;
        while(!lst.empty()) {
            auto& node = lst.front();
            if(node.updated()+idle_time>current) {
                break;
            }
            lst.splice(lst.begin(), lst, lst.end());
            node.set_updated(Timestamp::Now());
            node.callback();
        }
    }
}

IdleId Eventloop::RegisterIdle(int idle,
                               const TcpConnPtr& conn,
                               const TcpCallback& cb) {
    if (!idle_enabled_) {
        idle_enabled_ = true;
        RunEvery([=]() { callIdles(); }, Timestamp::kMicroSecondsPerSecond);
    }
    auto lst = idle_map_[idle];
    lst.push_back(IdleNode(conn, cb, Timestamp::Now()));
    return IdleId(new Idle(&lst, --lst.end()));
}

void Eventloop::UnRegisterIdle(const IdleId& idle) {
    if(idle_enabled_) {
        idle->lst()->erase(idle->iter());
    }
}

void Eventloop::UpdateIdle(const IdleId& idle) {
    if(idle_enabled_) {
        auto lst = idle->lst();
        auto iter = idle->iter();
        iter->set_updated(Timestamp::Now());
        lst->splice(lst->end(), *lst, iter);
    }
}

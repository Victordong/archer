#include "archer/net/poller/epoll_poller.hpp"
using namespace archer;

Eventloop::Eventloop() : poller_(new EpollPoller(*this)) {}

Eventloop::~Eventloop() {}

void Eventloop::loop() {
    quit_ = false;
    looping_ = true;

    while (!quit_) {
        int num = poller_->poll(kPollTimeMs, active_channels_);
        std::cout << num << std::endl;
        for (ChannelList::const_iterator const_iter = active_channels_.begin();
             const_iter != active_channels_.end() && num > 0;
             const_iter++, num--) {
            (*const_iter)->HandleEvent();
        }
    }
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
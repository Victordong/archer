#include "archer/net/poller/epoll_poller.hpp"

using namespace archer;

EpollPoller::EpollPoller()
    : epfd_(epoll_create1(EPOLL_CLOEXEC)),
      events_(EventList(kInitSize_)) {
    if (epfd_.fd() < 0) {
    }
}

EpollPoller::~EpollPoller() {}

void EpollPoller::Poll(int timeout_ms, ChannelList& active_channels) {
    int num_events =
        epoll_wait(epfd(), &*events_.begin(), events_.size(), timeout_ms);

    if (num_events > 0) {
        fillActiveChannels(num_events, active_channels);
    } else if (num_events == 0) {
    } else {
    }

    if (num_events == events_.size()) {
        events_.resize(2 * events_.size());
    }
}

void EpollPoller::fillActiveChannels(int num_events,
                                     ChannelList& active_channels) const {
    ChannelMap::const_iterator channel_iter;
    Channel* cur_channel;

    for (EventList::const_iterator ev_iter = events_.begin();
         ev_iter != events_.end() && num_events > 0; ++ev_iter) {
        --num_events;
        channel_iter = channels_.find(ev_iter->data.fd);
        cur_channel = channel_iter->second;
        cur_channel->set_revents(ev_iter->events);
        active_channels.push_back(cur_channel);
    }
}

void EpollPoller::UpdateChannel(Channel& channel) {
    epoll_event ev;
    ev.events = channel.events();
    ev.data.fd = channel.fd();
    int result = epoll_ctl(epfd(), EPOLL_CTL_MOD, channel.fd(), &ev);
}

void EpollPoller::RemoveChannel(Channel& channel) {
    channels_.erase(channel.fd());
    epoll_ctl(epfd(), EPOLL_CTL_DEL, channel.fd(), nullptr);
}

void EpollPoller::AddChannel(Channel& channel) {
    channels_[channel.fd()] = &channel;
    epoll_event ev;
    ev.events = channel.events();
    ev.data.fd = channel.fd();
    int result = epoll_ctl(epfd(), EPOLL_CTL_ADD, channel.fd(), &ev);
}

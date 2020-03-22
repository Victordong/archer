#include "archer/net/eventloop/channel.hpp"

using namespace archer;

Channel::Channel(Eventloop& loop, int fd)
    : loop_(loop), fd_(new Socket(fd)), events_(0), revents_(0) {
    loop_.AddChannel(*this);
}

Channel::~Channel() {
    loop_.RemoveChannel(*this);
}

void Channel::update() {
    loop_.UpdateChannel(*this);
}

void Channel::HandleEvent() {
    if (revents_ & kNoneEvent) {
    }
    if (revents_ & kErrorEvent) {
        error_callback_();
    }
    if (revents_ & kReadEvent) {
        read_callback_();
    }
    if (revents_ & kWriteEvent) {
        write_callback_();
    }
}

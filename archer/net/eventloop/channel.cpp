#include "archer/net/eventloop/channel.hpp"

using namespace archer;

Channel::Channel(Eventloop* loop, int fd)
    : loop_(loop), fd_(fd), events_(0), revents_(0) {}

Channel::~Channel() {
    this->Remove();
}

void Channel::update() {
    loop_->UpdateChannel(*this);
}

void Channel::Remove() {
    loop_->RemoveChannel(*this);
}

void Channel::handleEvent() {
    if (revents_ & kNoneEvent) {
    }
    if (revents_ & kErrorEvent) {
        if (error_callback_) {
            error_callback_();
        }
    }
    if (revents_ & kReadEvent) {
        if (read_callback_) {
            read_callback_();
        }
    }
    if (revents_ & kWriteEvent) {
        if (write_callback_) {
            write_callback_();
        }
    }
}

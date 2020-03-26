#ifndef _ARCHER_CHANNEL_HPP
#define _ARCHER_CHANNEL_HPP

#include <functional>
#include <vector>
#include "archer/net/eventloop/eventloop.hpp"

namespace archer {

class Channel : noncopyable {
   public:
    Channel(Eventloop* loop, int fd);
    ~Channel();

    void HandleEvent();

    void set_read_callback(const EventCallback& cb) { read_callback_ = cb; };
    void set_write_callback(const EventCallback& cb) { write_callback_ = cb; };
    void set_error_callback(const EventCallback& cb) { error_callback_ = cb; };

    int fd() const { return fd_.fd(); };
    int events() const { return events_; };

    int revents() const { return revents_; };
    void set_revents(int revents) { revents_ = revents; };

    void EnableReading() {
        events_ |= kReadEvent;
        update();
    };

    void EnableWriting() {
        events_ |= kWriteEvent;
        update();
    };

    void DisableReading() {
        events_ &= ~kWriteEvent;
        update();
    };

    void DisableAll() {
        events_ = kErrorEvent;
        update();
    }

    Eventloop* eventloop() { return loop_; };

   private:
    void update();

    Eventloop* loop_;
    const Socket fd_;
    int events_;
    int revents_;
    int index_;

    EventCallback read_callback_;
    EventCallback write_callback_;
    EventCallback error_callback_;
};

};      // namespace archer
#endif  // _ARCHER_CHANNEL_HPP
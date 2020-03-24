#include <string.h>
#include <sys/timerfd.h>
#include <memory>
#include "archer/net/eventloop/channel.hpp"

using namespace archer;

archer::Eventloop* g_loop;

void timeout() {
    std::cout << "Time out" << std::endl;
    g_loop->quit();
}

int main(int argc, char const* argv[]) {
    archer::Eventloop loop;

    g_loop = &loop;

    int timerfd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
    archer::Channel channel(&loop, timerfd);
    loop.AddChannel(channel);
    channel.set_read_callback(timeout);
    channel.EnableReading();

    itimerspec howlong;
    bzero(&howlong, sizeof howlong);
    howlong.it_value.tv_sec = 1;
    int result = timerfd_settime(timerfd, 0, &howlong, nullptr);

    loop.Loop();

    close(timerfd);
    std::cout << "end" << std::endl;
    return 0;
}

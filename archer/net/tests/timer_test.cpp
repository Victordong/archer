#include <string.h>
#include <sys/timerfd.h>
#include <memory>
#include "archer/net/eventloop/channel.hpp"

using namespace archer;

int main(int argc, char const* argv[]) {
    archer::Eventloop loop;
    loop.RunEvery([]() { std::cout << "hello" << std::endl; },
                  5 * Timestamp::kMicroSecondsPerSecond);
    loop.Loop();
    return 0;
}
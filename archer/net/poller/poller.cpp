#include "archer/net/eventloop/eventloop.hpp"
using namespace archer;

#ifdef __linux__
#include "archer/net/poller/epoll_poller.hpp"

Poller::Poller() : imp_(new EpollPoller()) {}
#endif  //

Poller::~Poller() {}

#include "archer/base/socket.hpp"
#include "assert.h"
using namespace archer;

Socket::Socket(int domain, int type, int protocol) {
    fd_ = ::socket(domain, type, protocol);
    assert(fd_ > 0);
}

void Socket::SetNonBlock(bool value) {
    int flags = ::fcntl(fd_, F_GETFL, 0);
    int result;
    if (value) {
        result = ::fcntl(fd_, F_SETFL, flags | O_NONBLOCK);
    } else {
        result = ::fcntl(fd_, F_SETFL, flags & ~O_NONBLOCK);
    }
    assert(result >= 0);
}

void Socket::SetReusePort(bool value) {
    int flag = value;
    int len = sizeof flag;
    int result = ::setsockopt(fd_, SOL_SOCKET, SO_REUSEPORT, &flag, len);
    assert(result >= 0);
}

void Socket::SetReuseAddr(bool value) {
    int flag = value;
    int len = sizeof flag;
    int result = ::setsockopt(fd_, SOL_SOCKET, SO_REUSEADDR, &flag, len);
    assert(result >= 0);
}

void Socket::SetNoDelay(bool value) {
    int flag = value;
    int len = sizeof flag;
    int result = ::setsockopt(fd_, SOL_SOCKET, TCP_NODELAY, &flag, len);
    assert(result >= 0);
}

void Socket::AddFlag(int flag) {
    int ret = fcntl(fd_, F_GETFD);
    int result = ::fcntl(fd_, F_SETFD, ret | flag);
    assert(result >= 0);
}

void Socket::Bind(struct sockaddr* addr) {
    int result = ::bind(fd_, addr, sizeof *addr);
    assert(result >= 0);
}

void Socket::Listen(int backlog_size) {
    int result = ::listen(fd_, backlog_size);
    assert(result >= 0);
}

int Socket::Accept(struct sockaddr* addr) {
    socklen_t len = sizeof(*addr);
    return ::accept(fd_, addr, &len);
}
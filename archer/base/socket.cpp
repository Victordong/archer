#include "archer/base/socket.hpp"
using namespace archer;

Socket::Socket(int domain, int type, int protocol) {
    fd_ = ::socket(domain, type, protocol);
}

void Socket::SetNonBlock(bool value) {
    int flags = ::fcntl(fd_, F_GETFL, 0);
    if (value) {
        ::fcntl(fd_, F_SETFL, flags | O_NONBLOCK);
    } else {
        ::fcntl(fd_, F_SETFL, flags & ~O_NONBLOCK);
    }
}

void Socket::SetReusePort(bool value) {
    int flag = value;
    int len = sizeof flag;
    ::setsockopt(fd_, SOL_SOCKET, SO_REUSEPORT, &flag, len);
}

void Socket::SetReuseAddr(bool value) {
    int flag = value;
    int len = sizeof flag;
    ::setsockopt(fd_, SOL_SOCKET, SO_REUSEADDR, &flag, len);
}

void Socket::SetNoDelay(bool value) {
    int flag = value;
    int len = sizeof flag;
    ::setsockopt(fd_, SOL_SOCKET, TCP_NODELAY, &flag, len);
}

void Socket::AddFdFlag(int flag) {
    int ret = fcntl(fd_, F_GETFD);
    ::fcntl(fd_, F_SETFD, ret | flag);
}

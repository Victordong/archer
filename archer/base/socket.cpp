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

void Socket::Bind( struct sockaddr* addr) {
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

Ip4Addr::Ip4Addr(const std::string& host, unsigned short port) {
    ::memset(&addr_, 0, sizeof addr_);
    addr_.sin_family = AF_INET;
    addr_.sin_port = port;
    if (host.size()) {
        GetHostName(host);
    } else {
        addr_.sin_addr.s_addr = INADDR_ANY;
    }
}

void Ip4Addr::GetHostName(const std::string & host) {
    struct hostent hent;
    struct hostent* he = nullptr;
    char buf[1024];
    int herro = 0;

    ::memset(&hent, 0, sizeof hent);
    int r = gethostbyname_r(host.c_str(), &hent, buf, sizeof buf, &he, &herro);
    if(r==0&&he&&he->h_addrtype==AF_INET) {
        addr_.sin_addr = *reinterpret_cast<struct in_addr*>(he->h_addr);
    } else {
        addr_.sin_addr.s_addr = INADDR_NONE;
    }
}

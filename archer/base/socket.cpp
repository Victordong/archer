#include "archer/base/socket.hpp"

#include <assert.h>

#include <iostream>

using namespace archer;

Socket::Socket(int domain, int type, int protocol) {
    fd_ = ::socket(domain, type, protocol);
    assert(fd_ > 0);
}

int Socket::SetNonBlock(int fd, bool value) {
    int flags = ::fcntl(fd, F_GETFL, 0);
    int result;
    if (value) {
        result = ::fcntl(fd, F_SETFL, flags | O_NONBLOCK);
    } else {
        result = ::fcntl(fd, F_SETFL, flags & ~O_NONBLOCK);
    }
    return result;
}

int Socket::SetReusePort(int fd, bool value) {
    int flag = value;
    int len = sizeof(flag);
    int result = ::setsockopt(fd, SOL_SOCKET, SO_REUSEPORT, &flag, len);
    return result;
}

int Socket::SetReuseAddr(int fd, bool value) {
    int flag = value;
    int len = sizeof(flag);
    int result = ::setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &flag, len);
    return result;
}

int Socket::SetNoDelay(int fd, bool value) {
    int flag = value;
    int len = sizeof(flag);
    int result = ::setsockopt(fd, SOL_SOCKET, TCP_NODELAY, &flag, len);
    return result;
}

int Socket::SetKeepAlive(int fd, bool value) {
    int optval = value ? 1 : 0;
    int result = ::setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, &optval,
                              static_cast<socklen_t>(sizeof(optval)));
    return result;
}

int Socket::AddFlag(int fd, int flag) {
    int ret = fcntl(fd, F_GETFD);
    int result = ::fcntl(fd, F_SETFD, ret | flag);
    return result;
}

int Socket::Bind(int fd, struct sockaddr* addr) {
    int result = ::bind(fd, addr, sizeof(struct sockaddr));
    return result;
}

int Socket::Connect(int fd, struct sockaddr* addr) {
    int result = ::connect(fd, addr, sizeof(struct sockaddr));
    return result;
}

int Socket::Listen(int backlog_size) {
    int result = ::listen(fd_, backlog_size);
    return result;
}

int Socket::Accept(struct sockaddr* addr) {
    socklen_t len = sizeof(*addr);
    int conn_fd = ::accept(fd_, addr, &len);
    SetNonBlock(conn_fd, true);
    AddFlag(conn_fd, FD_CLOEXEC);
    return conn_fd;
}

int Socket::GetLocalAddr(int fd, Ip4Addr& addr) {
    socklen_t local_len = sizeof(addr.addr());
    int result = ::getsockname(fd, (sockaddr*)&addr.addr(), &local_len);
    return result;
}

int Socket::GetPeerAddr(int fd, Ip4Addr& addr) {
    socklen_t local_len = sizeof(addr.addr());
    int result = ::getpeername(fd, (sockaddr*)&addr.addr(), &local_len);
    return result;
}

Ip4Addr::Ip4Addr(const std::string& host, unsigned short port) {
    ::memset(&addr_, 0, sizeof(addr_));
    addr_.sin_family = AF_INET;
    addr_.sin_port = htons(port);
    if (host.size()) {
        GetHostName(host);
    } else {
        addr_.sin_addr.s_addr = INADDR_ANY;
    }
}

void Ip4Addr::GetHostName(const std::string& host) {
    struct hostent hent;
    struct hostent* he = nullptr;
    char buf[1024];
    int herro = 0;

    ::memset(&hent, 0, sizeof(hent));
    int r = gethostbyname_r(host.c_str(), &hent, buf, sizeof(buf), &he, &herro);
    if (r == 0 && he && he->h_addrtype == AF_INET) {
        addr_.sin_addr = *reinterpret_cast<struct in_addr*>(he->h_addr);
    } else {
        addr_.sin_addr.s_addr = INADDR_NONE;
    }
}

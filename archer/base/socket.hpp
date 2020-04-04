#ifndef _ARCHER_SOCKET_HPP
#define _ARCHER_SOCKET_HPP

#include <fcntl.h>
#include <memory.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string>

namespace archer {

class Ip4Addr {
   public:
    Ip4Addr() { addr_.sin_addr.s_addr = INADDR_NONE; };
    Ip4Addr(const std::string& host, unsigned short port);
    Ip4Addr(unsigned short port) : Ip4Addr("", port){};
    Ip4Addr(const sockaddr_in& addr) : addr_(addr){};

    std::string ToString() const;
    std::string ip() const;
    unsigned short port() const;
    unsigned int ip_int() const;

    bool IsIpValid() const { return addr_.sin_addr.s_addr != INADDR_NONE; };
    struct sockaddr_in& addr() {
        return addr_;
    };

    static std::string HostToIp(std::string& host);

    void GetHostName(const std::string& host);

   private:
    struct sockaddr_in addr_;
};

class Socket {
   public:
    explicit Socket(int fd) : fd_(fd){};
    Socket(int domain, int type, int protocol = 0);
    ~Socket() { ::close(fd_); };

    int fd() const { return fd_; };

    void SetNonBlock(bool value = true) { SetNonBlock(fd_, value); };
    static void SetNonBlock(int fd, bool value = true);

    void SetReuseAddr(bool value = true) { SetReuseAddr(fd_, value); };
    static void SetReuseAddr(int fd, bool value = true);

    void SetReusePort(bool value = true) { SetReusePort(fd_, value); };
    static void SetReusePort(int fd, bool value = true);

    void SetNoDelay(bool value = true) { SetNoDelay(fd_, value); };
    static void SetNoDelay(int fd, bool value = true);

    void SetKeepAlive(bool value = true) { SetKeepAlive(fd_, value); };
    static void SetKeepAlive(int fd, bool value = true);

    void AddFlag(int flag) { AddFlag(fd_, flag); };
    static void AddFlag(int fd, int flag);

    void Bind(struct sockaddr*);
    void Bind(struct sockaddr_in* addr) { return Bind((sockaddr*)addr); };
    void Listen(int backlog_size = 20);

    int Accept(struct sockaddr*);
    int Accept(struct sockaddr_in* addr) { return Accept((sockaddr*)addr); };

    static void Close(int fd) { ::close(fd); };

    static Ip4Addr GetLocalAddr(int fd);
    static Ip4Addr GetPeerAddr(int fd);

   private:
    int fd_;
};

};  // namespace archer

#endif  // _ARCHER_SOCKET_HPP

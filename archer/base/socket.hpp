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

    int SetNonBlock(bool value = true) { return SetNonBlock(fd_, value); };
    static int SetNonBlock(int fd, bool value = true);

    int SetReuseAddr(bool value = true) { return SetReuseAddr(fd_, value); };
    static int SetReuseAddr(int fd, bool value = true);

    int SetReusePort(bool value = true) { return SetReusePort(fd_, value); };
    static int SetReusePort(int fd, bool value = true);

    int SetNoDelay(bool value = true) { return SetNoDelay(fd_, value); };
    static int SetNoDelay(int fd, bool value = true);

    int SetKeepAlive(bool value = true) { return SetKeepAlive(fd_, value); };
    static int SetKeepAlive(int fd, bool value = true);

    int AddFlag(int flag) { return AddFlag(fd_, flag); };
    static int AddFlag(int fd, int flag);

    int Bind(struct sockaddr* addr) { return Bind(fd_, addr); };
    int Bind(struct sockaddr_in* addr) { return Bind(fd_, addr); };
    static int Bind(int fd, struct sockaddr*);
    static int Bind(int fd, sockaddr_in* addr) {
        return Bind(fd, (sockaddr*)addr);
    };

    static int Connect(int fd, struct sockaddr*);
    static int Connect(int fd, sockaddr_in* addr){
        return Connect(fd, (sockaddr*)addr);
    };

    int Listen(int backlog_size = 20);

    int Accept(struct sockaddr*);
    int Accept(struct sockaddr_in* addr) { return Accept((sockaddr*)addr); };

    static int Close(int fd) { return ::close(fd); };

    int GetLocalAddr(Ip4Addr& addr) { return GetLocalAddr(fd_, addr); };
    static int GetLocalAddr(int fd, Ip4Addr& addr);
    int GetPeerAddr(Ip4Addr& addr) { return GetPeerAddr(fd_, addr); };
    static int GetPeerAddr(int fd, Ip4Addr &addr);

   private:
    int fd_;
};

};  // namespace archer

#endif  // _ARCHER_SOCKET_HPP

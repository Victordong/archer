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
class Socket {
   public:
    explicit Socket(int fd) : fd_(fd){};
    Socket(int domain, int type, int protocol = 0);
    ~Socket() { ::close(fd_); };

    int fd() const { return fd_; };

    void SetNonBlock(bool value = true);
    void SetReuseAddr(bool value = true);
    void SetReusePort(bool value = true);
    void SetNoDelay(bool value = true);

    void AddFlag(int flag);

    void Bind(struct sockaddr*);
    void Bind(struct sockaddr_in* addr) { return Bind((sockaddr*)addr); };
    void Listen(int backlog_size = 20);

    int Accept(struct sockaddr*);
    int Accept(struct sockaddr_in* addr) { return Accept((sockaddr*)addr); };

   private:
    int fd_;
};

class Ip4Addr {
   public:
    Ip4Addr(const std::string& host, unsigned short port);
    Ip4Addr(unsigned short port) : Ip4Addr("", port){};
    Ip4Addr(const sockaddr_in& addr) : addr_(addr){};

    std::string ToString() const;
    std::string ip() const;
    unsigned short port() const;
    unsigned int ip_int() const;

    bool IsIpValid() const;
    struct sockaddr_in& addr() {
        return addr_;
    };

    static std::string HostToIp(std::string& host);

    void GetHostName(const std::string& host);

   private:
    struct sockaddr_in addr_;
};

};  // namespace archer

#endif  // _ARCHER_SOCKET_HPP

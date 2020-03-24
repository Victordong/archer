#ifndef _ARCHER_SOCKET_HPP
#define _ARCHER_SOCKET_HPP

#include <unistd.h>
class Socket {
   public:
    Socket(int fd) : fd_(fd){};
    ~Socket() { ::close(fd_);};

    int fd() { return fd_; };

   private:
    int fd_;
};

#endif  // _ARCHER_SOCKET_HPP

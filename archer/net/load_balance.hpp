#ifndef _ARCHER_LOAD_BALANCE
#define _ARCHER_LOAD_BALANCE

#include "archer/base/noncopyable.hpp"
#include "archer/net/eventloop/eventloop.hpp"

namespace archer {
class LoadBalanceABC : noncopyable {
   public:
    LoadBalanceABC(){};
    ~LoadBalanceABC(){};

    virtual Eventloop* GetEventLoop(
        const TcpServer*,
        std::vector<std::unique_ptr<Eventloop>>&) = 0;
};

class RoundRoBinLB : public LoadBalanceABC {
   public:
    RoundRoBinLB(){};
    ~RoundRoBinLB(){};

    virtual Eventloop* GetEventLoop(const TcpServer*,
                                    std::vector<std::unique_ptr<Eventloop>>&);
};

class LeastConnectionLB : public LoadBalanceABC {
    public:
     LeastConnectionLB(){};
     ~LeastConnectionLB(){};

     virtual Eventloop* GetEventLoop(const TcpServer*,
                                     std::vector<std::unique_ptr<Eventloop>>&);
};

class RandomLB : public LoadBalanceABC {
    public:
     RandomLB(){};
     ~RandomLB(){};
     virtual Eventloop* GetEventLoop(const TcpServer*,
                                     std::vector<std::unique_ptr<Eventloop>>&);
};

};  // namespace archer

#endif  // _ARCHER_LOAD_BALANCE

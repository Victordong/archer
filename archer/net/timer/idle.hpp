#ifndef _ARCHER_IDLE
#define _ARCHER_IDLE

#include "archer/net/archer_imp.hpp"

namespace archer {
class IdleNode {
   public:
    IdleNode(const TcpConnPtr& conn, const TcpCallback& cb, Timestamp updated);
    ~IdleNode();

   private:
    TcpConnPtr conn_;
    TcpCallback cb_;
    Timestamp updated_;
};
class Idle {
    public:
     Idle(IdleList* lst, IdleIter iter);
     ~Idle();

    private:
     IdleList* lst_;
     IdleIter iter_;
};
};  // namespace archer

#endif  // _ARCHER_IDLE

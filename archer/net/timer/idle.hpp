#ifndef _ARCHER_IDLE
#define _ARCHER_IDLE

#include "archer/net/archer_imp.hpp"

namespace archer {
class IdleNode {
   public:
    IdleNode(const TcpConnPtr& conn, const TcpCallback& cb, Timestamp updated);
    ~IdleNode();

    Timestamp updated() { return updated_; };
    void set_updated(const Timestamp& time) { updated_ = time; };

    void callback() { return cb_(conn_); };

   private:
    TcpConnPtr conn_;
    TcpCallback cb_;
    Timestamp updated_;
};
class Idle {
    public:
     Idle(IdleList* lst, IdleIter iter);
     ~Idle();

     IdleList* lst() { return lst_; };
     IdleIter iter() { return iter_; };

    private:
     IdleList* lst_;
     IdleIter iter_;
};
};  // namespace archer

#endif  // _ARCHER_IDLE

#include "archer/net/timer/idle.hpp"
using namespace archer;

IdleNode::IdleNode(const TcpConnPtr& conn,
                   const TcpCallback& cb,
                   Timestamp updated)
    : conn_(conn), cb_(cb), updated_(updated) {}

IdleNode::~IdleNode(){};

Idle::Idle(IdleList* lst, IdleIter iter) : lst_(lst), iter_(iter) {}

Idle::~Idle() {}

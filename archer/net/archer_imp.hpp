#ifndef _ARCHER_IMP
#define _ARCHER_IMP

#include <assert.h>
#include <sys/epoll.h>
#include <sys/timerfd.h>
#include <unistd.h>

#include <atomic>
#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <mutex>
#include <list>
#include <vector>

#include "archer/base/buffer.hpp"
#include "archer/base/socket.hpp"
#include "archer/base/timestamp.hpp"

namespace archer {

class Acceptor;
class TcpServer;
class TcpConn;
class CodecImp;

class Poller;
class Channel;
class TimerQueue;
class Timer;

class IdleNode;
class Idle;

using IdleList = std::list<IdleNode>;
using IdleIter = std::list<IdleNode>::iterator;
using IdleId = std::shared_ptr<Idle>;
using IdleMap = std::map<int, IdleList>;
using IdleIdList = std::vector<IdleId>;

using SocketPtr = std::unique_ptr<Socket>;
using Functor = std::function<void()>;

using TimerPtr = std::shared_ptr<Timer>;
using TimerId = std::shared_ptr<Timer>;

using ChannelPtr = std::shared_ptr<Channel>;

using BufferPtr = std::unique_ptr<Buffer>;

using TcpServerPtr = std::shared_ptr<TcpServer>;
using TcpConnPtr = std::shared_ptr<TcpConn>;
using AcceptorPtr = std::shared_ptr<Acceptor>;
using CodecImpPtr = std::shared_ptr<CodecImp>;

using TcpCreateCallback = std::function<TcpConnPtr()>;
using TcpCallback = std::function<void(const TcpConnPtr&)>;
using TcpMsgCallBack = std::function<void(const TcpConnPtr&, const Slice&)>;
using RetMsgCallBack =
    std::function<std::string(const TcpConnPtr&, const std::string& msg)>;

using TimerCallback = std::function<void()>;
using EventCallback = std::function<void()>;

}  // namespace archer

#endif  // _ARCHER_IMP

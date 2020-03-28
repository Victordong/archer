#ifndef _ARCHER_TCP_CONN_HPP
#define _ARCHER_TCP_CONN_HPP

#include "archer/base/buffer.hpp"
#include "archer/net/codec.hpp"
#include "archer/net/eventloop/channel.hpp"
#include "archer/net/eventloop/eventloop.hpp"
#include "string.h"
#include "archer/net/reactor.hpp"

namespace archer {
class TcpServer;
class TcpConn;

using TcpServerPtr = std::shared_ptr<TcpServer>;
using TcpConnPtr = std::shared_ptr<TcpConn>;
using AcceptorPtr = std::shared_ptr<Acceptor>;
using SubReactorPtr = std::shared_ptr<SubReactor>;

using SubReactorList = std::vector<std::shared_ptr<SubReactor>>;

using TcpCallback = std::function<void(const TcpConnPtr&)>;
using TcpMsgCallBack = std::function<void(const TcpConnPtr&, Slice)>;


class TcpConn : noncopyable {
   public:
    enum ConnState {
        Invalid,
        Handshaking,
        Connected,
        Closed,
        Failed,
    };

    TcpConn();
    ~TcpConn();

    static TcpConnPtr CreateConnection(Eventloop* loop,
                                       const std::string& host,
                                       unsigned short port,
                                       int timeout = 0);

    static TcpConnPtr CreateConnection(Eventloop* loop, int fd);

    bool isClient();

    Eventloop* loop() { return loop_; };
    ConnState state() { return state_; };
    ChannelPtr channel() { return channel_; };
    bool writeable() {
        return channel_ ? channel_->revents() & archer::kWriteEvent : false;
    };
    void set_reconnect_interval(int milli) { reconnect_interval_ = milli; };
    Buffer& output() { return output_; };
    Buffer& input() { return input_; };

    void Send(Buffer& msg);
    void SendOutPut() { Send(output_); };
    void Send(const char* msg, size_t len);
    void Send(const std::string& msg) { Send(msg.data(), msg.size()); };
    void Send(const char* msg) { Send(msg, strlen(msg)); };

    void OnRead(const TcpCallback& cb, CodecImp* codec = nullptr);
    void OnWrite(const TcpCallback& cb) { writcb_ = cb; };
    void OnState(const TcpCallback& cb) { statecb_ = cb; };

    void Close();
    void CloseNow();

    void HandleRead(const TcpConnPtr& conn);
    void HandleWrite(const TcpConnPtr& conn);

    void Cleanup(const TcpConnPtr& conn);
    void Connect(Eventloop*, const std::string&, unsigned short, int);
    void ReConnect();
    void Attach(Eventloop* loop, int fd);

   private:
    Eventloop* loop_;
    ChannelPtr channel_;
    ConnState state_;

    Buffer output_, input_;

    TcpCallback readcb_, writcb_, statecb_;

    TimerId timerout_id_;

    std::string dest_host_, local_ip_;
    int dest_port_, connection_timeout_, reconnect_interval_;
    int64_t connected_time_;
};
};  // namespace archer

#endif  // _ARCHER_TCP_CONN_HPP

#ifndef _ARCHER_TCP_CONN_HPP
#define _ARCHER_TCP_CONN_HPP

#include "archer/base/buffer.hpp"
#include "archer/net/codec.hpp"
#include "archer/net/eventloop/channel.hpp"
#include "archer/net/eventloop/eventloop.hpp"
#include "archer/net/reactor.hpp"
#include "string.h"

namespace archer {
class TcpServer;
class TcpConn;

using TcpServerPtr = std::shared_ptr<TcpServer>;
using TcpConnPtr = std::shared_ptr<TcpConn>;
using AcceptorPtr = std::shared_ptr<Acceptor>;
using SubReactorPtr = std::shared_ptr<SubReactor>;
using CodecImpPtr = std::shared_ptr<CodecImp>;

using SubReactorList = std::vector<std::shared_ptr<SubReactor>>;

using TcpCallback = std::function<void(const TcpConnPtr&)>;
using TcpMsgCallBack = std::function<void(const TcpConnPtr&, Slice)>;

class TcpConn : public std::enable_shared_from_this<TcpConn>, noncopyable {
   public:
    enum ConnState {
        Invalid,
        Handshaking,
        Connected,
        Closed,
        Error,
    };

    TcpConn();
    ~TcpConn();

    static TcpConnPtr CreateConnection(Eventloop* loop,
                                       const std::string& host,
                                       unsigned short port,
                                       int timeout = 0,
                                       const std::string& local_ip = "") {
        TcpConnPtr conn = std::make_shared<TcpConn>(TcpConn());
        conn->Connect(loop, host, port, timeout, local_ip);
        return conn;
    }

    static TcpConnPtr CreateConnection(Eventloop* loop,
                                       int fd,
                                       Ip4Addr local_addr,
                                       Ip4Addr peer_addr) {
        TcpConnPtr conn = std::make_shared<TcpConn>(TcpConn());
        conn->attach(loop, fd, local_addr, peer_addr);
        return conn;
    }

    bool isClient() { return dest_port_ > 0; };

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

    void OnRead(const TcpCallback& cb) {readcb_ = cb;};
    void OnWrite(const TcpCallback& cb) { writcb_ = cb; };
    void OnState(const TcpCallback& cb) { statecb_ = cb; };
    void OnMsg(CodecImp* codec, const TcpMsgCallBack& cb);
    void OnClose(const TcpCallback& cb) { closecb_ = cb; };

    void Close();
    void CloseNow() {
        if (channel_) {
            channel_->Close();
        }
    };

    void HandleRead(const TcpConnPtr& conn);
    void HandleWrite(const TcpConnPtr& conn);
    void HandleClose(const TcpConnPtr& conn);
    void HandleError(const TcpConnPtr& conn);
    void HandleHandShake(const TcpConnPtr& conn);

    void Cleanup(const TcpConnPtr& conn);
    void Connect(Eventloop* loop,
                 const std::string& host,
                 unsigned short port,
                 int timeout,
                 const std::string& local_ip);
    void ReConnect();
    void attach(Eventloop* loop, int fd, Ip4Addr local, Ip4Addr peer);

    virtual int ReadImp(int fd, void* buf, size_t size) {
        return ::read(fd, buf, size);
    }
    virtual int WriteImp(int fd, const void* buf, size_t size) {
        return ::write(fd, buf, size);
    }

   private:
    Eventloop* loop_;
    ChannelPtr channel_;
    ConnState state_;

    Buffer output_, input_;

    TcpCallback readcb_, writcb_, statecb_, closecb_;
    TcpMsgCallBack msgcb_;

    TimerId timerout_id_;

    std::string dest_host_, local_ip_;

    Ip4Addr local_, peer_;

    std::unique_ptr<CodecImp> codec_;

    int dest_port_, connection_timeout_, reconnect_interval_;
    int64_t connected_time_;
};
};  // namespace archer

#endif  // _ARCHER_TCP_CONN_HPP

#ifndef _ARCHER_TCP_CONN_HPP
#define _ARCHER_TCP_CONN_HPP

#include <string.h>

#include "archer/base/buffer.hpp"
#include "archer/net/acceptor.hpp"
#include "archer/net/codec.hpp"
#include "archer/net/eventloop/channel.hpp"
#include "archer/net/eventloop/eventloop.hpp"

namespace archer {


static size_t kMallocSize = 1024;

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

    void Send(const char* msg, size_t len);
    void Send(Buffer& msg);
    void Send(const std::string& msg) { Send(msg.data(), msg.size()); };
    void Send(const char* msg) { Send(msg, strlen(msg)); };

    void SendOutPut() { Send(output_); };

    void SendMsg(Slice &s);
    void SendMsg(const std::string& s) { SendMsg(Slice(s)); };

    void OnRead(const TcpCallback& cb) { readcb_ = cb; };
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

    void handleRead(const TcpConnPtr& conn);
    void handleWrite(const TcpConnPtr& conn);
    void handleClose(const TcpConnPtr& conn);
    void handleError(const TcpConnPtr& conn);
    void handleHandShake(const TcpConnPtr& conn);

    void Cleanup(const TcpConnPtr& conn);

    void Connect(Eventloop* loop,
                 const std::string& host,
                 unsigned short port,
                 int timeout,
                 const std::string& local_ip);
    void ReConnect();

    void attach(Eventloop* loop, int fd, Ip4Addr local, Ip4Addr peer);

    virtual int readImp(int fd, void* buf, size_t size) {
        return ::read(fd, buf, size);
    }
    virtual int writeImp(int fd, const void* buf, size_t size) {
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

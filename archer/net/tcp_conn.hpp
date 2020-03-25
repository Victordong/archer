#include "archer/base/buffer.hpp"
#include "archer/net/eventloop/channel.hpp"
#include "archer/net/eventloop/eventloop.hpp"
#include "string.h"

namespace archer {
class TcpConn;

using TcpConnPtr = std::shared_ptr<TcpConn>;

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

    void Send(Buffer& msg);
    void SendOutPut() { Send(output_); };
    void Send(const char* msg, size_t len);
    void Send(const std::string& msg) { Send(msg.data(), msg.size()); };
    void Send(const char* msg) { Send(msg, strlen(msg)); };

    void OnRead(const TcpCallback& cb) {
        assert(!readcb_);
        readcb_ = cb;
    };
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

#ifndef _ARCHER_TCP_SERVER_HPP
#define _ARCHER_TCP_SERVER_HPP

#include "archer/net/codec.hpp"
#include "archer/net/eventloop/eventloop.hpp"
#include "archer/net/reactor.hpp"

namespace archer {

class TcpServer : noncopyable {
   public:
    TcpServer(int num);
    ~TcpServer();

    static TcpServerPtr StartServer(const std::string& host,
                                    unsigned short port,
                                    bool reuse_port = false);

    Ip4Addr addr() { return addr_; };

    void OnConnCreate(const std::function<TcpConnPtr()>& cb) { conncb_ = cb; };
    void OnRead(const TcpCallback& cb) { readcb_ = cb; };
    void OnState(const TcpCallback& cb) { statecb_ = cb; };
    void OnMsg(const TcpMsgCallBack& cb, CodecImp* codec) {
        codec_.reset(codec);
        msgcb_ = cb;
    }

   private:
    Acceptor acceptor;
    std::vector<SubReactor> reactors;

    Ip4Addr addr_;

    TcpCallback readcb_, statecb_;
    TcpMsgCallBack msgcb_;
    std::function<TcpConnPtr()> conncb_;

    std::unique_ptr<CodecImp> codec_;

    int backlog_size_ = 20;
};

};  // namespace archer

#endif  // _ARCHER_TCP_SERVER_HPP
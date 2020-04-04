#ifndef _ARCHER_TCP_SERVER_HPP
#define _ARCHER_TCP_SERVER_HPP

#include "archer/net/codec.hpp"
#include "archer/net/eventloop/eventloop.hpp"
#include "archer/net/reactor.hpp"

namespace archer {

class TcpServer : noncopyable {
   public:
    TcpServer(int num, const std::string& host = "", unsigned short port = 8080)
        : addr_(host, port), reactors_num_(num){};
    ~TcpServer();

    static TcpServerPtr InitServer(int num,
                                   const std::string& host,
                                   unsigned short port,
                                   bool reuse_port = false);

    Ip4Addr addr() { return addr_; };

    void OnConnCreate(const std::function<TcpConnPtr()>& cb) { conncb_ = cb; };
    void OnState(const TcpCallback& cb) { statecb_ = cb; };
    void OnRead(const TcpCallback& cb) { readcb_ = cb; };
    void OnMsg(const TcpMsgCallBack& cb, CodecImp* codec) {
        codec_.reset(codec);
        msgcb_ = cb;
    }

    void HandleAccept(int fd, Ip4Addr local_addr, Ip4Addr peer_addr);

    SubReactor* GetSubReactor();

    void Start();

   private:
    int reactors_num_;

    std::unique_ptr<Acceptor> acceptor;
    std::vector<std::unique_ptr<SubReactor>> reactors;

    Ip4Addr addr_;

    TcpCallback readcb_, statecb_;
    TcpMsgCallBack msgcb_;
    std::function<TcpConnPtr()> conncb_;

    std::map<int, TcpConnPtr> conn_map_;

    std::shared_ptr<CodecImp> codec_;

    int backlog_size_ = 20;
    std::atomic_uint64_t total_connections_;
};

};  // namespace archer

#endif  // _ARCHER_TCP_SERVER_HPP
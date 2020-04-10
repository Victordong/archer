#ifndef _ARCHER_ACCEPTOR_HPP
#define _ARCHER_ACCEPTOR_HPP

#include "archer/net/eventloop/eventloop.hpp"

namespace archer {
class TcpServer;

class Acceptor {
   public:
    Acceptor(const Ip4Addr& addr,
             bool reuse_port = true,
             int backlog_size = 20);

    Acceptor(const std::string& host,
             unsigned short port,
             bool reuse_port = false,
             int block_size = 20)
        : Acceptor(Ip4Addr(host, port), reuse_port, block_size){};
    ~Acceptor();

    Eventloop* loop() { return loop_.get(); };

    void RunLoop() { loop_->Loop(); };

    void Listen() { listenfd_.Listen(); };

    void handleAccept();

    void set_new_conncb_(const std::function<void(int, Ip4Addr, Ip4Addr)>& cb) {
        conncb_ = cb;
    };

   private:
    Socket listenfd_;

    TcpServer* server;

    std::unique_ptr<Eventloop> loop_;
    std::unique_ptr<Channel> listen_channel_;

    std::function<void(int, Ip4Addr, Ip4Addr)> conncb_;

    Ip4Addr addr_;
};
};  // namespace archer

#endif  // _ARCHER_ACCEPTOR_HPP

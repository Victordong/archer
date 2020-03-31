#ifndef _ARCHER_REACTOR_HPP
#define _ARCHER_REACTOR_HPP

#include "archer/net/eventloop/eventloop.hpp"
#include "archer/net/tcp_conn.hpp"

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

    void HandleAccept();

    void set_new_conncb_(const std::function<TcpConnPtr()>& cb) {
        conncb_ = cb;
    };

   private:
    Socket listenfd_;

    TcpServer* server;

    std::unique_ptr<Eventloop> loop_;
    std::unique_ptr<Channel> listen_channel_;

    std::function<TcpConnPtr()> conncb_;

    Ip4Addr addr_;
};

class SubReactor {
   public:
    SubReactor();
    ~SubReactor();

   private:
    std::unique_ptr<Eventloop> loop_;

    std::atomic_uint64_t total_nums_;
};

};  // namespace archer

#endif  // _ARCHER_REACTOR_HPP

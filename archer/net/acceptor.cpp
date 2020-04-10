#include "archer/net/acceptor.hpp"

#include "archer/net/eventloop/channel.hpp"
#include "archer/net/tcp_server.hpp"
using namespace archer;

Acceptor::Acceptor(const Ip4Addr& addr, bool reuse_port, int backlog_size)
    : addr_(addr),
      listenfd_(AF_INET, SOCK_STREAM),
      loop_(new Eventloop()),
      listen_channel_(new Channel(loop_.get(), listenfd_.fd())) {
    listen_channel_->EnableReading();
    listen_channel_->set_read_callback([=]() { this->handleAccept(); });
    loop_->AddChannel(*listen_channel_);

    listenfd_.SetNonBlock(true);
    listenfd_.SetReuseAddr(true);
    listenfd_.SetReusePort(reuse_port);
    listenfd_.AddFlag(FD_CLOEXEC);
    listenfd_.Bind(&addr_.addr());
}

void Acceptor::handleAccept() {
    Ip4Addr peer_addr;
    int conn_fd = listenfd_.Accept(&peer_addr.addr());
    if (conn_fd > 0) {
        if (conncb_) {
            Ip4Addr local_addr = Socket::GetLocalAddr(conn_fd);
            conncb_(conn_fd, local_addr, peer_addr);
        } else {
            Socket::Close(conn_fd);
        }
    } else {
    }
}

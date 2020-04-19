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

    int result = listenfd_.SetNonBlock(true);
    result = listenfd_.SetReuseAddr(true);
    result = listenfd_.SetReusePort(reuse_port);
    result = listenfd_.AddFlag(FD_CLOEXEC);
    result = listenfd_.Bind(&addr_.addr());
}

void Acceptor::handleAccept() {
    Ip4Addr peer_addr;
    int conn_fd = listenfd_.Accept(&peer_addr.addr());
    if (conn_fd > 0) {
        if (conncb_) {
            Ip4Addr local_addr;
            int result = Socket::GetLocalAddr(conn_fd, local_addr);
            conncb_(conn_fd, local_addr, peer_addr);
        } else {
            Socket::Close(conn_fd);
        }
    } else {
    }
}

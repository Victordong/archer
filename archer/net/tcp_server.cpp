#include "archer/net/tcp_server.hpp"

#include <stdlib.h>
#include <time.h>
using namespace archer;

TcpServerPtr TcpServer::InitServer(int num,
                                   const std::string& host,
                                   unsigned short port,
                                   bool reuse_port,
                                   LoadBalanceABC* lb) {
    auto server = std::make_shared<TcpServer>(num, host, port, lb);
    server->acceptor_.reset(new Acceptor(host, port, reuse_port));
    server->acceptor_->set_new_conncb_(
        [server](int fd, Ip4Addr local_addr, Ip4Addr peer_addr) {
            server->handleAccept(fd, local_addr, peer_addr);
        });
    for (auto& reactor : server->reactors_) {
        reactor = std::unique_ptr<Eventloop>(new Eventloop);
    }

    return server;
}

void TcpServer::handleAccept(int fd, Ip4Addr local_addr, Ip4Addr peer_addr) {
    auto conn = this->conncb_();
    conn_map_.insert(std::pair<int, TcpConnPtr>(fd, conn));

    auto reactor = load_balance_->GetEventLoop(this, reactors_);

    reactor->RunInLoop([=]() {
        if (statecb_) {
            conn->OnState(statecb_);
        }

        if (readcb_) {
            conn->OnRead(readcb_);
        }

        if (msgcb_) {
            conn->OnMsg(codec_->Clone(), msgcb_);
        }

        conn->attach(reactor, fd, local_addr, peer_addr);

        conn->OnClose([=](const TcpConnPtr& conn) { handleClose(conn); });

        reactor->total()++;
    });

    total_connections_++;
}

void TcpServer::handleClose(const TcpConnPtr& conn) {
    conn_map_.erase(conn->channel()->fd());
    if (closecb_) {
        closecb_(conn);
    }
}

void TcpServer::Start() {
    acceptor_->Listen();

    std::vector<std::thread> threads;
    for (auto& reactor : reactors_) {
        threads.push_back(std::thread([&]() { reactor->Loop(); }));
    }
    acceptor_->RunLoop();
}

HSHAPtr HSHA::InitServer(int num,
                         int pool_size,
                         const std::string& host,
                         unsigned short port,
                         bool reuse_port) {
    auto server = std::make_shared<HSHA>(num, pool_size, host, port);

    server->acceptor_.reset(new Acceptor(host, port, reuse_port));
    server->acceptor_->set_new_conncb_(
        [server](int fd, Ip4Addr local_addr, Ip4Addr peer_addr) {
            server->handleAccept(fd, local_addr, peer_addr);
        });
    for (auto& reactor : server->reactors_) {
        reactor = std::unique_ptr<Eventloop>(new Eventloop);
    }

    return server;
}

void HSHA::Start() {
    TcpServer::Start();
}

void HSHA::OnConnMsg(const RetMsgCallBack& cb, CodecImp* codec) {
    TcpServer::OnConnMsg(
        [=](const TcpConnPtr& conn, const Slice& slice) {
            std::string msg = slice;
            thread_pool_.AddTask([=]() {
                auto result = cb(conn, msg);
                conn->loop()->RunInLoop([=]() { conn->SendMsg(Slice(result)); });
            });
        },
        codec);
}
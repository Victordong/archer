#include "archer/net/tcp_server.hpp"
using namespace archer;

TcpServerPtr TcpServer::InitServer(int num,
                                   const std::string& host,
                                   unsigned short port,
                                   bool reuse_port) {
    TcpServerPtr server =
        std::make_shared<TcpServer>(TcpServer(num, host, port));
    server->acceptor.reset(new Acceptor(host, port, reuse_port));
    server->acceptor->set_new_conncb_(
        [server](int fd, Ip4Addr local_addr, Ip4Addr peer_addr) {
            server->HandleAccept(fd, local_addr, peer_addr);
        });
    for (int i = 0; i < num; i++) {
        server->reactors_.push_back(std::unique_ptr<Eventloop>(new Eventloop));
    }
    return server;
}

void TcpServer::HandleAccept(int fd, Ip4Addr local_addr, Ip4Addr peer_addr) {
    TcpConnPtr conn = this->conncb_();
    conn_map_.insert(std::pair<int, TcpConnPtr>(fd, conn));
    Eventloop* reactor = GetSubReactor();

    reactor->RunInLoop([=]() {
        conn->attach(reactor, fd, local_addr, peer_addr);

        if (readcb_) {
            conn->OnWrite(readcb_);
        }

        if(statecb_) {
            conn->OnState(statecb_);
        }

        if (readcb_) {
            conn->OnRead(readcb_);
        } 

        if (msgcb_) {
            conn->OnMsg(codec_->clone(), msgcb_);
        }

        conn->OnClose([=](const TcpConnPtr& conn) { HandleClose(conn); });

        reactor->total()++;
    });

    total_connections_++;
}

void TcpServer::HandleClose(const TcpConnPtr& conn) {
    acceptor->loop()->RunInLoop([=]() {
        conn_map_.erase(conn->channel()->fd());
        if(closecb_) {
            closecb_(conn);
        }
    });
}

void TcpServer::Start() {}
#include "archer/net/tcp_server.hpp"

int main(int argc, char const* argv[]) {
    auto server = archer::TcpServer::InitServer(3, "", 10800);
    server->OnConnRead([](const archer::TcpConnPtr& conn) { 
        conn->Send(conn->input()); });
    server->Start();
    return 0;
}

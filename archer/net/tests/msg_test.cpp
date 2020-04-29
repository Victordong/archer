#include "archer/net/tcp_server.hpp"

int main(int argc, char const* argv[]) {
    auto server = archer::HSHA::InitServer(2, 2, "", 10800);

    server->OnConnCreate([]() -> archer::TcpConnPtr {
        std::cout << "conn begin" << std::endl;
        return std::make_shared<archer::TcpConn>();
    });

    server->OnConnState([](const archer::TcpConnPtr& conn) {
        switch (conn->state()) {
            case archer::ConnState::Connected:
                std::cout << "connected" << std::endl;
                // conn->AddIdleCB(
                //     10, [](const archer::TcpConnPtr& conn) { conn->Close(); });
                break;
            case archer::ConnState::Closed:
                std::cout << "closed" << std::endl;
                break;
            case archer::ConnState::Error:
                std::cout << "error" << std::endl;
                break;
            default:
                break;
        }
    });

    server->OnConnClose([](const archer::TcpConnPtr& conn) {
        std::cout << "conn close" << std::endl;
    });

    server->OnConnMsg(
        [](const archer::TcpConnPtr& conn,
           const std::string& msg) -> std::string {
            std::cout << msg << std::endl;
            return msg;
        },
        new archer::LineCodec);

    server->Start();

    return 0;
}

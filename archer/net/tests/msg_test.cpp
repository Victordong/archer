#include "archer/base/logger/logger.hpp"
#include "archer/net/tcp_server.hpp"

int main(int argc, char const* argv[]) {
    auto server = archer::HSHA::InitServer(2, 2, "", 10800);

    server->OnConnCreate([]() -> archer::TcpConnPtr {
        return std::make_shared<archer::TcpConn>();
    });

    server->OnConnState([](const archer::TcpConnPtr& conn) {
        switch (conn->state()) {
            case archer::ConnState::Connected:
                archer::INFO_LOG("%s", "连接建立");
                conn->AddIdleCB(10, [](const archer::TcpConnPtr& conn) {
                    archer::INFO_LOG("%s", "连接不活跃");
                    conn->Close();
                });
                break;
            case archer::ConnState::Error:
                archer::ERR_LOG("%s", "连接出现错误");
                break;
            default:
                break;
        }
    });

    server->OnConnClose([](const archer::TcpConnPtr& conn) {
        archer::INFO_LOG("%s", "连接关闭");
    });

    server->OnConnMsg(
        [](const archer::TcpConnPtr& conn,
           const std::string& msg) -> std::string {
            archer::INFO_LOG("%s", "接受消息，并回应");
            archer::INFO_LOG("%s", msg.c_str());
            return msg;
        },
        new archer::LineCodec);

    server->Start();

    return 0;
}

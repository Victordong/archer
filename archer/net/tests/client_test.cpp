#include "archer/net/tcp_server.hpp"
#include "archer/net/tcp_conn.hpp"
using namespace archer;

int main(int argc, char const* argv[]) {
    Eventloop loop;
    auto conn = archer::TcpConn::CreateConnection(&loop, "", 10800);
    conn->OnState([](const archer::TcpConnPtr& conn) {
        if (conn->state()==archer::ConnState::Connected) {
            conn->SendMsg("test");
        }
    });
    conn->OnMsg(new LineCodec(), [](const TcpConnPtr& conn, const Slice& s) {
        std::string msg = s.ToString();
        conn->SendMsg(msg);
        std::cout << msg << std::endl;
    });
    loop.Loop();
}

#include "archer/net/load_balance.hpp"

#include "archer/net/tcp_server.hpp"
using namespace archer;

Eventloop* RoundRoBinLB::GetEventLoop(
    const TcpServer* server,
    std::vector<std::unique_ptr<Eventloop>>& loops) {
    auto number = server->total_connections() % server->reactors_num();
    return loops[number].get();
}

Eventloop* LeastConnectionLB::GetEventLoop(
    const TcpServer* server,
    std::vector<std::unique_ptr<Eventloop>>& loops) {
    auto number = 0;
    for (int i = 1; i < loops.size(); i++) {
        if (loops[i]->total() < loops[number]->total()) {
            number = i;
        }
    }
    return loops[number].get();
}

Eventloop* RandomLB::GetEventLoop(
    const TcpServer* server,
    std::vector<std::unique_ptr<Eventloop>>& loops) {
    srand((unsigned)time(NULL));
    auto number = rand() % 4;
    return loops[number].get();
}

#ifndef _ARCHER_TCP_SERVER_HPP
#define _ARCHER_TCP_SERVER_HPP

#include "archer/base/thread.hpp"
#include "archer/net/codec.hpp"
#include "archer/net/eventloop/eventloop.hpp"
#include "archer/net/load_balance.hpp"
#include "archer/net/tcp_conn.hpp"

namespace archer {

enum LoadMode { RoundRoBin, LeastConnections, Random };

class TcpServer : noncopyable {
   public:
    TcpServer(int num,
              const std::string& host = "",
              unsigned short port = 8080,
              LoadBalanceABC* lb = new RoundRoBinLB())
        : addr_(host, port),
          reactors_num_(num),
          reactors_(num),
          load_balance_(lb),
          conncb_([]() -> TcpConnPtr { return std::make_shared<TcpConn>(); }){};

    ~TcpServer(){};

    static TcpServerPtr InitServer(int num,
                                   const std::string& host,
                                   unsigned short port,
                                   bool reuse_port = false,
                                   LoadBalanceABC* lb = new RoundRoBinLB());

    Ip4Addr addr() { return addr_; };

    virtual void OnConnCreate(const TcpCreateCallback& cb) { conncb_ = cb; };
    virtual void OnConnState(const TcpCallback& cb) { statecb_ = cb; };
    virtual void OnConnRead(const TcpCallback& cb) { readcb_ = cb; }
    virtual void OnConnMsg(const TcpMsgCallBack& cb, CodecImp* codec) {
        codec_.reset(codec);
        msgcb_ = cb;
    }
    virtual void OnConnClose(const TcpCallback& cb) { closecb_ = cb; };

    virtual void handleAccept(int fd, Ip4Addr local_addr, Ip4Addr peer_addr);
    virtual void handleClose(const TcpConnPtr& conn);

    virtual void Start();

    uint64_t total_connections() const { return total_connections_; };

    uint64_t reactors_num() const { return reactors_num_; };

   protected:
    uint64_t reactors_num_;

    std::unique_ptr<Acceptor> acceptor_;
    std::vector<std::unique_ptr<Eventloop>> reactors_;

    Ip4Addr addr_;
    std::unique_ptr<LoadBalanceABC> load_balance_;

    TcpCallback readcb_, statecb_, closecb_;
    TcpMsgCallBack msgcb_;
    std::function<TcpConnPtr()> conncb_;

    std::map<int, TcpConnPtr> conn_map_;

    std::shared_ptr<CodecImp> codec_;

    int backlog_size_ = 20;
    std::atomic_uint64_t total_connections_;
};

class HSHA;

using HSHAPtr = std::shared_ptr<HSHA>;

class HSHA : public TcpServer {
   public:
    HSHA(int el_num,
         int pool_size,
         const std::string& host = "",
         unsigned short port = 8080)
        : TcpServer(el_num, host, port), thread_pool_(pool_size){};

    static HSHAPtr InitServer(int num,
                              int pool_size,
                              const std::string& host,
                              unsigned short port,
                              bool reuse_port = false);

    void Exit() {
        thread_pool_.Exit();
        thread_pool_.Join();
    };

    virtual void OnConnMsg(const RetMsgCallBack& cb, CodecImp* codec);

    virtual void Start();

   private:
    ThreadPool thread_pool_;
};

};  // namespace archer

#endif  // _ARCHER_TCP_SERVER_HPP
#include "archer/net/tcp_conn.hpp"
using namespace archer;

TcpConn::TcpConn() : state_(ConnState::Invalid) {}

TcpConn::~TcpConn() {}

void TcpConn::attach(Eventloop* loop, int fd, Ip4Addr local, Ip4Addr peer) {
    loop_ = loop;
    local_ = local;
    peer_ = peer;
    state_ = ConnState::Handshaking;

    TcpConnPtr conn = shared_from_this();

    channel_.reset(new Channel(loop, fd));
    loop_->AddChannel(*channel_);
    channel_->set_read_callback([=]() { conn->handleRead(conn); });
    channel_->set_write_callback([=]() { conn->handleWrite(conn); });
    channel_->set_error_callback([=]() { conn->handleError(conn); });
    channel_->EnableReading();

    handleHandShake(conn);

    Socket::SetKeepAlive(channel_->fd());
}

void TcpConn::OnMsg(CodecImp* codec, const TcpMsgCallBack& cb) {
    codec_.reset(codec);
    OnRead([=](const TcpConnPtr& conn) {
        int len;
        do {
            Slice msg;
            len = codec_->TryDecode(conn->input_, msg);
            if (len < 0) {
                handleClose(conn);
            } else if (len > 0) {
                cb(conn, msg);
                conn->input_.Consume(len);
            }
        } while (len);
    });
}

void TcpConn::handleRead(const TcpConnPtr& conn) {
    if (state_ == ConnState::Connected) {
        for (auto& idle : lst_) {
            updateIdle(idle);
        }
        input_.malloc(kMallocSize);
        int result = readImp(channel_->fd(), input_.end(), input_.space());
        if (result < 0) {
            if (errno == EAGAIN || errno == EINTR || errno == EWOULDBLOCK) {
                return;
            } else {
                handleError(conn);
            }
        } else if (result == 0) {
            handleClose(conn);
        } else {
            readcb_(conn);
        }
    }
}

void TcpConn::handleWrite(const TcpConnPtr& conn) {
    if (state_ == ConnState::Connected) {
        int result = writeImp(channel_->fd(), output_.begin(), output_.size());
        if (result > 0) {
            output_.Consume(result);
        } else if (result < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR) {
            } else {
                if (channel_->WriteEnabled()) {
                    channel_->DisableWriting();
                    handleError(conn);
                }
            }
        } else {
        }
        if (output_.empty() && channel_->WriteEnabled()) {
            channel_->DisableWriting();
        }
    }
}

void TcpConn::handleClose(const TcpConnPtr& conn) {
    if (state_ == ConnState::Connected) {
        state_ = ConnState::Closed;

        for (auto& idle_id : lst_) {
            unregisterIdle(idle_id);
        }

        channel_->Remove();
        channel_.reset();

        loop_->total()--;

        statecb_(conn);
        closecb_(conn);

        readcb_ = writcb_ = statecb_ = closecb_ = nullptr;
    }
}

void TcpConn::handleError(const TcpConnPtr& conn) {
    if (state_ != Error) {
        state_ = ConnState::Error;
        channel_->Remove();
        loop_->total()--;
        statecb_(conn);
        closecb_(conn);
    }
}

void TcpConn::handleHandShake(const TcpConnPtr& conn) {
    if(conn->isClient()) {
         
    } else {
        state_ = ConnState::Connected;
        statecb_(conn);
    }
}

void TcpConn::Send(const char* msg, size_t len) {
    loop_->RunInLoop([&]() {
        if (channel_) {
            output_.Append(msg, len);
            if (output_.size() && !channel_->WriteEnabled()) {
                channel_->WriteEnabled();
            }
        }
    });
}

void TcpConn::Send(Buffer& msg) {
    loop_->RunInLoop([&]() {
        if (channel_) {
            output_.Append(msg);
            if (output_.size() && !channel_->WriteEnabled()) {
                channel_->WriteEnabled();
            }
        }
    });
}

void TcpConn::SendMsg(Slice& msg) {
    codec_->Encode(msg, output_);
    SendOutPut();
}

void TcpConn::Connect(Eventloop* loop,
                      const std::string& host,
                      unsigned short port,
                      int timeout,
                      const std::string& local_ip) {
    dest_host_ = host;
    dest_port_ = port;
    connection_timeout_ = timeout;
    connected_time_ = Timestamp::Now();
    local_ip_ = local_ip;

    int fd = ::socket(AF_INET, SOCK_STREAM, 0);

    int result = Socket::SetNonBlock(fd, true);

    result = Socket::AddFlag(fd, FD_CLOEXEC);

    Ip4Addr dest_addr(host, port);

    if (local_ip.size()) {
        Ip4Addr src_addr(local_ip, 0);
        Socket::Bind(fd, &src_addr.addr());
    }

    if(result==0) {
        result = Socket::Connect(fd, &dest_addr.addr());
    }

    auto src_addr = Socket::GetLocalAddr(fd);

    state_ = ConnState::Handshaking;
    attach(loop, fd, src_addr, dest_addr);
    if (timeout) {
        auto tcp_conn = shared_from_this();
        timerout_id_ = loop_->RunAfter([&]() {
            if (tcp_conn->state() == ConnState::Handshaking) {
                handleClose(tcp_conn);
            }
        }, timeout);
    }
}

void TcpConn::ReConnect() {}

void TcpConn::Close() {}

void TcpConn::Cleanup(const TcpConnPtr& conn) {}

void TcpConn::AddIdleCB(int idle, const TcpCallback& cb) {
    auto idle_id = loop_->RegisterIdle(idle, shared_from_this(), cb);
    lst_.push_back(idle_id);
}

void TcpConn::unregisterIdle(const IdleId& idle) {
    loop_->UnRegisterIdle(idle);
}

void TcpConn::updateIdle(const IdleId& idle) {
    loop_->UpdateIdle(idle);
}

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
                conn->channel()->Close();
            } else if (len > 0) {
                cb(conn, msg);
                conn->input_.Consume(len);
            }
        } while (len);
    });
}

void TcpConn::handleRead(const TcpConnPtr& conn) {
    if (state_ == ConnState::Connected) {
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
                conn->channel_->DisableWriting();
                handleError(conn);
            }
        } else {
        }
        if (output_.empty()) {
            channel_->DisableWriting();
        }
    }
}

void TcpConn::handleClose(const TcpConnPtr& conn) {
    if (state_ == ConnState::Connected) {
        state_ = ConnState::Closed;
        channel_->Remove();
        loop_->total()--;
        closecb_(conn);
    }
}

void TcpConn::handleError(const TcpConnPtr& conn) {
    if (state_ != Error) {
        state_ = ConnState::Error;
        channel_->Remove();
        closecb_(conn);
    }
}

void TcpConn::handleHandShake(const TcpConnPtr& conn) {
    state_ = ConnState::Connected;
    statecb_(conn);
}

void TcpConn::Send(const char* msg, size_t len) {}

void TcpConn::SendMsg(Slice& s) {}

void TcpConn::Connect(Eventloop* loop,
                      const std::string& host,
                      unsigned short port,
                      int timeout,
                      const std::string& local_ip) {}

void TcpConn::ReConnect() {}

void TcpConn::Close() {}

void TcpConn::Cleanup(const TcpConnPtr& conn) {}

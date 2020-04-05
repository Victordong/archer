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
    channel_->set_read_callback([=]() { conn->HandleRead(conn); });
    channel_->set_write_callback([=]() { conn->HandleWrite(conn); });
    channel_->set_error_callback([=]() { conn->HandleError(conn); });
    channel_->EnableReading();

    HandleHandShake(conn);

    Socket::SetKeepAlive(channel_->fd());
}

void TcpConn::OnMsg(CodecImp* codec, const TcpMsgCallBack& cb) {
    codec_.reset(codec);
    OnRead([=](const TcpConnPtr& conn) {
        int len;
        do {
            Slice msg;
            len = codec_->tryDecode(conn->input(), msg);
            if (len<0) {
                conn->channel()->Close();
            } else if (len >0) {
                cb(conn, msg);
            }
        } while (len);
    });
}

void TcpConn::HandleRead(const TcpConnPtr& conn) {
    if (state_ == ConnState::Connected) {
        int result = ReadImp(channel_->fd(), loop_->buffer(), kBufferSize);
        if (result <0) {
            if (errno == EAGAIN || errno == EINTR || errno == EWOULDBLOCK) {
                return;
            } else {
                HandleError(conn);
            }
        } else if (result ==0) {
            HandleClose(conn);
        } else {
            readcb_(conn);
        }
    }
}

void TcpConn::HandleWrite(const TcpConnPtr& conn) {}

void TcpConn::HandleClose(const TcpConnPtr& conn) {
    if (state_==ConnState::Connected) {
        state_ = ConnState::Closed;
        channel_->Remove();
        closecb_(conn);
    }
}

void TcpConn::HandleError(const TcpConnPtr& conn) {
    if (state_!=Error) {
        state_ = ConnState::Error;
        channel_->Remove();
        closecb_(conn);
    }
}
    

void TcpConn::HandleHandShake(const TcpConnPtr& conn) {
    state_ = ConnState::Connected;
    statecb_(conn);
}

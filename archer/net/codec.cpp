#include "archer/net/codec.hpp"

#include <netinet/in.h>
using namespace archer;

void LineCodec::Encode(Slice& msg, Buffer& buf) {
    buf.Append(msg).Append("\r\n");
}

int LineCodec::TryDecode(Slice data, Slice& msg) {
    if (data.size() == 1 && data[0] == 0x04) {
        msg = data;
        return 1;
    }
    for (size_t i = 0; i < data.size(); i++) {
        if (data[i] == '\n') {
            if (i > 0 && data[i - 1] == '\r') {
                msg = Slice(data.data(), i - 1);
                return i + 1;
            } else {
                msg = Slice(data.data(), i);
                return i + 1;
            }
        }
    }
    return 0;
}
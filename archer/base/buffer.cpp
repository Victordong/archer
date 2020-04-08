#include "archer/base/buffer.hpp"
using namespace archer;

Buffer& Buffer::append(const char* p, size_t len) {
    if (len > space()) {
        malloc(std::max(2 * cap_, len - space()));
    }
    if (write_pos_ + len > cap_) {
        auto right = cap_ - write_pos_;
        auto left = (write_pos_ + len) % cap_;
        std::copy(p, p + right, data()+write_pos_);
        std::copy(p + right, p + len, data());
    } else {
        std::copy(p, p + len, end());
    }
    write_pos_ = (write_pos_ + len) % cap_;
    return *this;
}

Buffer& Buffer::malloc(size_t len) {
    int new_size;
    if (len == 0) {
        if (new_size < kInitSize * kInitSize) {
            new_size = cap_ * 2;
        } else {
            new_size = kInitSize * kInitSize + cap_;
        }
    } else {
        new_size = cap_ + len;
    }

    auto new_buf = std::vector<char>(new_size);
    if(read_pos_>write_pos_) {
        std::copy(begin(), data() + cap_, new_buf.data());
        std::copy(data(), end(), new_buf.data() + cap_ - read_pos_);
    } else {
        std::copy(begin(), end(), new_buf.data());
    }
    buf_ = std::move(new_buf);

    write_pos_ = (write_pos_ - read_pos_ + cap_) % cap_;
    read_pos_ = 0;
    cap_ = new_size;

    return *this;
}
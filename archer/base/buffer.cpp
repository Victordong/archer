#include "archer/base/buffer.hpp"
using namespace archer;

Buffer& Buffer::Append(const char* p, size_t len) {
    if (len > space()) {
        malloc(std::max(2 * cap_, len - space()));
    }
    std::copy(p, p + len, end());
    write_pos_ = write_pos_ + len;
    return *this;
}

Buffer& Buffer::malloc(size_t len) {
    if (space() + pre_space() > len) {
        std::copy(begin(), end(), data());
    } else {
        auto new_size = size() + len;
        auto new_buf = std::vector<char>(new_size);
        std::copy(begin(), end(), new_buf.data());
        buf_ = std::move(new_buf);
        cap_ = new_size;
    }
    write_pos_ = write_pos_ - read_pos_;
    read_pos_ = 0;
    return *this;
}

void Buffer::copyFrom(const Buffer& buf) {
    if (this != &buf) {
        buf_ = buf.buf_;
        cap_ = buf.cap_;
        write_pos_ = buf.write_pos_;
        read_pos_ = buf.read_pos_;
    }
}

void Buffer::moveFrom(Buffer&& buf) {
    if (this != &buf) {
        buf_ = std::move(buf.buf_);
        cap_ = buf.cap_;
        write_pos_ = buf.write_pos_;
        read_pos_ = buf.read_pos_;
    }
}

Buffer& Buffer::Append(Buffer& buf) {
    if (this != &buf) {
        Append(buf.data(), buf.size());
    }
    return *this;
}

inline Slice Slice::EatWord() {
    auto b_iter = begin_;
    while (b_iter < end_ && ::isspace(*b_iter)) {
        b_iter++;
    }
    auto e_iter = b_iter;
    while (e_iter < end_ && !::isspace(*e_iter)) {
        e_iter++;
    }
    begin_ = e_iter;
    return Slice(b_iter, e_iter);
}

inline Slice Slice::EatLine() {
    auto slice_begin = begin_;
    while (begin_ < end_ && *begin_ != '\r' && *begin_ != '\n') {
        begin_++;
    }
    return Slice(slice_begin, begin_);
}

inline Slice& Slice::TrimSpace() {
    while (begin_ < end_ && ::isspace(*begin_)) {
        begin_++;
    }
    while (end_ > begin_ && ::isspace(*end_)) {
        end_--;
    }
    return *this;
}

inline int Slice::compare(const Slice& slice) const {
    auto len = size(), s_len = slice.size();
    auto min_len = std::min(len, s_len);
    int r = ::memcmp(data(), slice.data(), min_len);
    if (r == 0) {
        if (len > s_len) {
            return 1;
        } else if (s_len > len) {
            return -1;
        }
    }
    return r;
}

inline bool operator<(const Slice& a, const Slice& b) {
    return a.compare(b) < 0;
}

inline bool operator>(const Slice& a, const Slice& b) {
    return a.compare(b) > 0;
}

inline bool operator>=(const Slice& a, const Slice& b) {
    return a.compare(b) >= 0;
}

inline bool operator<=(const Slice& a, const Slice& b) {
    return a.compare(b) <= 0;
}

inline bool operator==(const Slice& a, const Slice& b) {
    return a.compare(b) == 0;
}

inline bool operator!=(const Slice& a, const Slice& b) {
    return a.compare(b) != 0;
}

inline SliceList Slice::Split(char c) const {
    SliceList list;
    auto slice_begin = begin_;
    for (auto iter = begin_; iter < end_; iter++) {
        if (*iter == c) {
            list.push_back(Slice(slice_begin, iter));
            slice_begin = iter + 1;
        }
    }
    if (begin_ != end_) {
        list.push_back(Slice(slice_begin, end_));
    }
    return list;
}
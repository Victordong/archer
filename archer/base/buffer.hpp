#ifndef _ARCHER_BUFFER
#define _ARCHER_BUFFER
#include <stddef.h>
#include <string.h>

#include <string>
#include <vector>

namespace archer {
static int kInitSize = 1024;

class Slice;

using SliceList = std::vector<Slice>;

class Slice {
   public:
    Slice() : begin_(nullptr), end_(nullptr){};
    Slice(const char* begin, const char* end) : begin_(begin), end_(end){};
    Slice(const std::string& s) : begin_(s.data()), end_(s.data() + s.size()){};
    Slice(const char* begin, size_t len) : begin_(begin), end_(begin + len){};
    Slice(const char* begin) : begin_(begin), end_(begin + strlen(begin)){};
    ~Slice(){};

    const char* begin() const { return begin_; };
    const char* data() const { return begin_; };
    const char* end() const { return end_; };
    size_t size() const { return end_ - begin_; };
    void Resize(size_t len) { end_ = begin_ + len; };
    inline bool empty() const { return begin_ == end_; };
    inline char operator[](int i) { return *(begin_ + i); };
    void Clear() { begin_ = end_ = nullptr; };

    Slice EatWord();
    Slice EatLine();
    Slice Eat(size_t len) {
        auto old_begin = begin_;
        begin_ = begin_ + len;
        return Slice(old_begin, len);
    };

    Slice Sub(int boff, int eoff = 0) const {
        return Slice(begin_ + boff, end_ - eoff);
    };

    Slice& TrimSpace();

    SliceList Split(char c) const;

    std::string ToString() const { return std::string(begin_, end_); };

    int compare(const Slice&) const;

    bool StartWith(const Slice& slice) {
        return (size() >= slice.size()) &&
               (::memcmp(data(), slice.data(), slice.size()) == 0);
    };
    bool EndWith(const Slice& slice) {
        return (size() >= slice.size()) &&
               (::memcmp(end() - slice.size(), slice.data(), slice.size()) ==
                0);
    };
    operator std::string() const { return std::string(begin_, end_); };

   private:
    const char* begin_;
    const char* end_;
};

class Buffer {
   public:
    Buffer() : buf_(kInitSize), cap_(kInitSize), write_pos_(0), read_pos_(0){};

    Buffer(const Buffer& buf) { copyFrom(buf); };

    Buffer(Buffer&& buf) { moveFrom(std::move(buf)); };

    ~Buffer() { buf_.clear(); };

    void Clear() {
        write_pos_ = 0;
        read_pos_ = 0;
    }

    size_t size() const { return write_pos_ - read_pos_; };

    bool empty() { return write_pos_ == read_pos_; };

    bool full() { return write_pos_ - read_pos_ == cap_; };

    char* data() { return buf_.data(); };

    char* cap() { return buf_.data() + cap_; };

    char* begin() { return buf_.data() + read_pos_; };

    char* end() { return buf_.data() + write_pos_; };

    size_t space() const { return cap_ - write_pos_; };

    size_t pre_space() const { return read_pos_; };

    Buffer& malloc(size_t len = 0);

    Buffer& Append(const char* p, size_t len);

    Buffer& Append(Slice slice) { return Append(slice.data(), slice.size()); }

    Buffer& Append(const char* p) { return Append(p, sizeof(p)); };

    Buffer& Append(const std::string& s) { return Append(s.data(), s.size()); };

    Buffer& Append(Buffer& buf);

    Buffer& Consume(size_t len) {
        read_pos_ += len;
        if (size() == 0) {
            Clear();
        }
        return *this;
    };  // shiftN

    Buffer& AddSize(size_t len) {
        write_pos_ += len;
        return *this;
    };

    Buffer& operator=(const Buffer& buf) {
        if (&buf == this) {
            return *this;
        }
        copyFrom(buf);
        return *this;
    };

    Buffer& operator=(Buffer&& buf) {
        if (&buf == this) {
            return *this;
        }
        moveFrom(std::move(buf));
        return *this;
    };

    operator Slice() { return Slice(begin(), size()); };

   private:
    std::vector<char> buf_;
    size_t cap_, write_pos_, read_pos_;

    void copyFrom(const Buffer& buf);
    void moveFrom(Buffer&& buf);
};

};  // namespace archer

#endif  // _ARCHER_BUFFER

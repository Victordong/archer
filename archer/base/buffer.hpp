#ifndef _ARCHER_BUFFER
#define _ARCHER_BUFFER
#include <stddef.h>
#include <vector>
#include <string.h>

namespace archer {
static int kInitSize = 1024;

class Slice {
    public:
     Slice(char*, size_t);
     char* data();
     size_t size();

    private:
     char* slice_b_;
     size_t size_;
};

class Buffer {
   public:
    Buffer()
        : buf_(kInitSize),
          cap_(kInitSize),
          write_pos_(0),
          read_pos_(0),
          empty_(true){};

    Buffer(const Buffer& buf) { copyFrom(buf); };

    Buffer(Buffer&& buf);

    ~Buffer() { buf_.clear(); };

    void clear() {
        write_pos_ = 0;
        read_pos_ = 0;
        empty_ = true;
        buf_.clear();
    }

    size_t size() const {
        if(write_pos_-read_pos_!=0) {
            return (write_pos_ - read_pos_ + cap_) % cap_;
        } else {
            if(empty_) {
                return 0;
            } else {
                return cap_;
            }
        }
    }

    bool empty() { return empty_; };

    bool full() { return !empty_ && read_pos_ == write_pos_; };

    char* data() { return buf_.data(); };

    char* begin() { return buf_.data() + read_pos_; };

    char* end() { return buf_.data() + write_pos_; };

    size_t space() const { return cap_ - size(); }

    Buffer& malloc(size_t len);

    void malloc() {}

    Buffer& append(const char* p, size_t len);

    Buffer& append(Slice slice) { return append(slice.data(), slice.size()); }

    Buffer& append(const char* p) { return append(p, sizeof(p)); };

    Buffer& consume(size_t len) { 
        read_pos_ += len;
        if(size()==0) {
            clear();
        }
        return *this;
    };

    Buffer& absorb(Buffer& buf);

    Buffer& operator=(const Buffer& buf) { return *this; };

    Buffer& operator=(Buffer&& buf) { return *this; };

    operator Slice() { return Slice(begin(), size()); };

   private:
    std::vector<char> buf_;
    size_t cap_, write_pos_, read_pos_;
    bool empty_;

    void expand(size_t len);
    void copyFrom(const Buffer& b);
};

};  // namespace archer

#endif  // _ARCHER_BUFFER

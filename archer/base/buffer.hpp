#ifndef _ARCHER_BUFFER
#define _ARCHER_BUFFER
#include <stddef.h>
#include <vector>
#include <string.h>

namespace archer {
static int kInitSize = 1024;

class Slice {
    public:
     char* data();
     size_t size();
};

class Buffer {
   public:
    Buffer()
        : buf_(kInitSize),
          cap_(kInitSize),
          write_pos_(0),
          read_pos_(0),
          empty_(true){};
    ~Buffer() { buf_.clear(); };

    void clear() {
        write_pos_ = 0;
        read_pos_ = 0;
        buf_.clear();
    }

    size_t size() const {
        if(write_pos_-read_pos_!=0) {
            return (write_pos_ - read_pos_ + cap_) & cap_;
        } else {
            if(empty_) {
                return 0;
            } else {
                return cap_;
            }
        }
    }

    bool empty() { return empty_; };

    char* data() { return buf_.data() + read_pos_; };

    char* begin() { return buf_.data() + read_pos_; };

    char* end() { return buf_.data() + write_pos_; };

    size_t space() const { return cap_ - size(); }

    char* makeRoom(size_t len);

    void makeRoom() {}

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


    void readRaw(int* read, int* write);

    void ClearN(int n);

    void IsFull();

    void malloc(int cap);

    void reset();

   private:
    std::vector<char> buf_;
    size_t cap_, write_pos_, read_pos_;
    bool empty_;

    void moveHead() {
        std::copy(begin(), end(), buf_.data());
        write_pos_ = size();
        read_pos_ = 0;
    }

    void expand(size_t len);
    void copyFrom(const Buffer& b);
};

};  // namespace archer

#endif  // _ARCHER_BUFFER

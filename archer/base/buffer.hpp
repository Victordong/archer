#ifndef _ARCHER_BUFFER
#define _ARCHER_BUFFER
#include <stddef.h>
#include <vector>

class Buffer {
   public:
    Buffer(/* args */);
    ~Buffer();

   private:
    std::vector<char> buf;
    size_t size_, write_pos_, read_pos_;
    bool empty;
};

class Slice {};
#endif  // _ARCHER_BUFFER

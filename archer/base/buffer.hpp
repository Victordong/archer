#ifndef _ARCHER_BUFFER
#define _ARCHER_BUFFER
#include <stddef.h>

class Buffer {
   public:
    Buffer(/* args */);
    ~Buffer();

   private:
    char* buf;
    size_t size_, write_pos_, read_pos_;
};
#endif  // _ARCHER_BUFFER

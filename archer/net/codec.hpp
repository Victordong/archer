#ifndef _ARCHER_CODEC_HPP
#define _ARCHER_CODEC_HPP

#include "archer/base/noncopyable.hpp"
#include "archer/base/buffer.hpp"

namespace archer {
class CodecImp {
   public:
    CodecImp* clone();
    int tryDecode(const Buffer&, Slice& msg);

   private:
    /* data */
};

}  // namespace archer

#endif  // _ARCHER_CODEC_HPP

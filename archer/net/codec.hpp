#ifndef _ARCHER_CODEC_HPP
#define _ARCHER_CODEC_HPP

#include <string>

#include "archer/base/buffer.hpp"
#include "archer/base/noncopyable.hpp"

namespace archer {
class CodecImp {
   public:
    virtual CodecImp* Clone() = 0;
    virtual int TryDecode(Slice, Slice&) = 0;
    virtual void Encode(Slice& msg, Buffer& buf) = 0;
};

class LineCodec : public CodecImp {
   public:
    CodecImp* Clone() { return new LineCodec(); };
    int TryDecode(Slice, Slice&);
    void Encode(Slice& msg, Buffer& buf);
};


}  // namespace archer

#endif  // _ARCHER_CODEC_HPP

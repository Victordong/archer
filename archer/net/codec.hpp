#ifndef _ARCHER_CODEC_HPP
#define _ARCHER_CODEC_HPP

#include "archer/base/noncopyable.hpp"
#include "archer/base/buffer.hpp"
#include <string>

namespace archer {
class CodecImp {
   public:
    virtual CodecImp* clone() = 0;
    virtual int tryDecode(Buffer&, std::string&) = 0;
    virtual void encode(Slice& msg, Buffer& buf) = 0;
};

class LineCodec: public CodecImp {
   public:
    CodecImp* clone() { return new LineCodec(); };
    int tryDecode(Buffer&, std::string&);
    void encode(Slice& msg, Buffer& buf);
};

class LengthCodec : public CodecImp {
   public:
    CodecImp* clone() { return new LengthCodec(); };
    int tryDecode(Buffer&, std::string&);
    void encode(Slice& msg, Buffer& buf);
};

}  // namespace archer

#endif  // _ARCHER_CODEC_HPP

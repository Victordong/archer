#include "archer/net/codec.hpp"
using namespace archer;

void LengthCodec::encode(Slice& msg, Buffer& buf) {}

int LengthCodec::tryDecode(Buffer&, std::string&) {}

void LineCodec::encode(Slice& msg, Buffer& buf) {}

int LineCodec::tryDecode(Buffer&, std::string&) {}
#ifndef _ARCHER_NONCOPYABLE_HPP
#define _ARCHER_NONCOPYABLE_HPP
namespace archer {

class noncopyable {
   protected:
    noncopyable() = default;
    ~noncopyable() = default;
    noncopyable(const noncopyable&) = delete;
    noncopyable& operator=(const noncopyable&) = delete;
};

};  // namespace archer

#endif  // _ARCHER_NONCOPYABLE_HPP

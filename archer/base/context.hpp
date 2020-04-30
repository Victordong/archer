#ifndef _ARCHER_CONTEXT
#define _ARCHER_CONTEXT

#include <functional>

#include "archer/base/noncopyable.hpp"

namespace archer {
using Task = std::function<void()>;

class AutoContext : noncopyable {
   public:
    AutoContext() : ctx_(0){};
    ~AutoContext() {
        if (ctx_) {
            ctx_del_();
        }
    };

    template <class T>
    T& context() {
        if (ctx_ == nullptr) {
            ctx_ = new T();
            ctx_del_ = [=]() { delete (T*)ctx_; };
        }
        return *(T*)ctx_;
    };

   public:
    void* ctx_;
    Task ctx_del_;
};
};  // namespace archer

#endif  // _ARCHER_CONTEXT

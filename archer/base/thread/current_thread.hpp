#ifndef _ARCHER_CURRENT_THREAD_HPP
#define _ARCHER_CURRENT_THREAD_HPP

#include <sstream>
#include <thread>

namespace archer {

class CurrentThread final {
   public:
    static unsigned long long tid() {
        std::ostringstream oss;
        oss << std::this_thread::get_id();
        std::string stid = oss.str();
        unsigned long long tid = std::stoull(stid);
        return tid;
    };
};

};  // namespace archer

#endif  // _ARCHER_CURRENT_THREAD_HPP
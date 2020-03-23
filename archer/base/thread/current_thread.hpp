#ifndef _ARCHER_CURRENT_THREAD_HPP
#define _ARCHER_CURRENT_THREAD_HPP

#include <sstream>
#include <thread>

namespace archer {

class CurrentThread final {
   public:
    static pid_t tid() {
        std::ostringstream oss;
        oss << std::this_thread::get_id();
        std::string stid = oss.str();
        pid_t tid = std::stoull(stid);
        return tid;
    };
};

};  // namespace archer

#endif  // _ARCHER_CURRENT_THREAD_HPP
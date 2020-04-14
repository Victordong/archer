#ifndef _ARCHER_THREAD_HPP
#define _ARCHER_THREAD_HPP

#include <linux/unistd.h>
#include <sys/syscall.h>
#include <unistd.h>

#include <thread>
#include <vector>

#include "archer/base/safe_queue.hpp"

namespace archer {
using Task = std::function<void()>;

extern __thread int t_cachedTid;

class CurrentThread : noncopyable {
   public:
    static pid_t gettid() { return static_cast<pid_t>(::syscall(SYS_gettid)); }

    static void cacheTid(){
        if (t_cachedTid == 0) {
            t_cachedTid = gettid();
        }
    };

    static int tid() {
        if (t_cachedTid == 0) {
            cacheTid();
        }
        return t_cachedTid;
    };

    static bool isMainThread() { return tid() == ::getpid(); };
};

class ThreadPool : noncopyable {
   public:
    ThreadPool(int threads, bool start = true);
    ~ThreadPool();

    void Start();
    ThreadPool& Exit() {
        tasks_.Exit();
        return *this;
    }
    void Join();

    bool AddTask(Task&& task);
    bool AddTask(const Task& task);

    size_t task_size() { return tasks_.size(); };

   private:
    SafeQueue<Task> tasks_;
    std::vector<std::thread> threads_;
};

};  // namespace archer

#endif  // _ARCHER_THREAD_HPP

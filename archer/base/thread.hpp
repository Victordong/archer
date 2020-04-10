#ifndef _ARCHER_THREAD_HPP
#define _ARCHER_THREAD_HPP

#include <thread>
#include <vector>

#include "archer/base/safe_queue.hpp"

namespace archer {
using Task = std::function<void()>;

class ThreadPool {
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

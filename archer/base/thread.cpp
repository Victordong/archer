#include "archer/base/thread.hpp"
using namespace archer;

ThreadPool::ThreadPool(int threads, bool start):tasks_(), threads_(threads) {
    if(start) {
        Start();
    }
}

ThreadPool::~ThreadPool() {
    tasks_.Exit();
    if(tasks_.size()) {

    }
}

void ThreadPool::Start() {
    for(auto & th :threads_) {
        std::thread t([this]() {
            while(!tasks_.exited()) {
                Task t;
                if (tasks_.PopWait(&t)) {
                    t();
                }
            }
        });
        th.swap(t);
    }
}

void ThreadPool::Join() {
    for( auto & th: threads_) {
        th.join();
    }
}

bool ThreadPool::AddTask(Task&& task) {
    return tasks_.Push(std::move(task));
}

bool ThreadPool::AddTask(const Task& task) {
    return tasks_.Push(task);
}
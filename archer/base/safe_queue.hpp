#ifndef _ARCHER_SAFEQUEUE_HPP
#define _ARCHER_SAFEQUEUE_HPP

#include <archer/base/noncopyable.hpp>
#include <atomic>
#include <condition_variable>
#include <functional>
#include <limits>
#include <list>
#include <mutex>

namespace archer {
using Task = std::function<void()>;

template <typename T>
class SafeQueue : private std::mutex, private noncopyable {
    static const int wait_infinite = std::numeric_limits<int>::max();

   public:
    SafeQueue() : exit_(false){};

    bool Push(T&& v);
    bool Push(const T& v);

    T PopWait(int wait_ms = wait_infinite);
    bool PopWait(T* t, int wait_ms = wait_infinite);

    size_t size();
    bool exited() { return exit_; };

    void Exit();

   private:
    std::list<T> items_;
    std::condition_variable ready_;
    std::atomic<bool> exit_;
    void waitReady(std::unique_lock<std::mutex>& lk, int wait_ms);
};

template <typename T>
size_t SafeQueue<T>::size() {
    std::lock_guard<std::mutex> lk(*this);
    return items_.size();
}

template <typename T>
void SafeQueue<T>::Exit() {
    exit_ = true;
    std::lock_guard<std::mutex> lk(*this);
    ready_.notify_all();
}

template <typename T>
bool SafeQueue<T>::Push(T&& t) {
    std::lock_guard<std::mutex> lk(*this);
    if (!exit_) {
        return false;
    }
    items_.push_back(std::move(t));
    ready_.notify_one();
    return true;
}

template <typename T>
bool SafeQueue<T>::Push(const T& t) {
    std::lock_guard<std::mutex> lk(*this);
    if (!exit_) {
        return false;
    }
    items_.push_back(t);
    ready_.notify_one();
    return true;
}

template <typename T>
void SafeQueue<T>::waitReady(std::unique_lock<std::mutex>& lk, int wait_ms) {
    if (exit_ || items_.empty()) {
        return;
    }
    if (wait_ms == wait_infinite) {
        ready_.wait(lk, [this]() -> bool { return exit_ || !items_.empty(); });
    } else {
        auto tp = std::chrono::steady_clock::now() +
                  std::chrono::microseconds(wait_ms);
        while (ready_.wait_until(lk, tp) != std::cv_status::timeout &&
               items_.empty() && !exit_) {
        }
    }
}

template <typename T>
bool SafeQueue<T>::PopWait(T* t, int wait_ms) {
    std::unique_lock<std::mutex> lk(*this);
    waitReady(lk, wait_ms);
    if (items_.empty()) {
        return false;
    }
    *t = std::move(items_.front());
    items_.pop_front();
    return true;
}

template <typename T>
T SafeQueue<T>::PopWait(int wait_ms) {
    std::unique_lock<std::mutex> lk(*this);
    waitReady(lk, wait_ms);
    T temp = std::move(items_.front());
    items_.pop_front();
    return temp;
}

};  // namespace archer

#endif  // _ARCHER_SAFEQUEUE_HPP

#ifndef _ARCHER_TIMESTAMP_HPP
#define _ARCHER_TIMESTAMP_HPP
#include <sys/time.h>
#include <time.h>

#include <vector>

namespace archer {

using timestamp_t = long int;

class Timestamp final {
   public:
    Timestamp() : micro_seconds_(0){};

    explicit Timestamp(timestamp_t ms) : micro_seconds_(ms){};

    Timestamp(const Timestamp& ts) : micro_seconds_(ts.micro_seconds_){};

    ~Timestamp() = default;

    static const int kMicroSecondsPerSecond = 1000 * 1000;

    static const int kNanoSecondsPerSecond = 1000 * 1000 * 1000;

    static const int kNanoSecondsPerMicroSecond = 1000;

    bool operator<(const Timestamp& other) const {
        return micro_seconds_ < other.micro_seconds_;
    };

    bool operator>(const Timestamp& other) const {
        return micro_seconds_ > other.micro_seconds_;
    };

    bool operator==(const Timestamp& other) const {
        return micro_seconds_ == other.micro_seconds_;
    };

    Timestamp& operator+(int delay) {
        micro_seconds_ = micro_seconds_ + delay;
        return *this;
    }

    timestamp_t expiration() {
        struct timeval tv;
        gettimeofday(&tv, nullptr);
        timestamp_t ms = timestamp_t(tv.tv_sec) * kMicroSecondsPerSecond +
                         timestamp_t(tv.tv_usec);
        return micro_seconds_ - ms;
    };

    void swap(Timestamp& first, Timestamp& second) {
        using std::swap;
        swap(first.micro_seconds_, second.micro_seconds_);
    };

    static Timestamp Now() {
        struct timeval tv;
        gettimeofday(&tv, nullptr);
        timestamp_t ms = timestamp_t(tv.tv_sec) * kMicroSecondsPerSecond +
                         timestamp_t(tv.tv_usec);
        return Timestamp(ms);
    };

   private:
    timestamp_t micro_seconds_;
};

};  // namespace archer

#endif  // _ARCHER_TIMESTAMP_HPP

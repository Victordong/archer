#include "archer/base/logger/logger.hpp"

#include <thread>

using namespace archer;

Logger::Logger(LogCallBack cb) {
    std::thread t([this]() {
        while (!tasks_.exited()) {
            LogTask t;
            if (tasks_.PopWait(&t)) {
                log_cb_(t);
            }
        }
    });
}

Logger* Logger::GetLogger() {
    std::lock_guard<std::mutex> lk(lock_);
    if (logger_ == nullptr) {
        logger_ = new Logger();
    }
    return logger_;
}

void Logger::Info(const std::string& msg) { Log(LogLevel::LOG_INFO, msg); }

void Logger::Warn(const std::string& msg) { Log(LogLevel::LOG_WARN, msg); }

void Logger::Error(const std::string& msg) { Log(LogLevel::LOG_ERROR, msg); }

void Logger::Debug(const std::string& msg) { Log(LogLevel::LOG_DEBUG, msg); }

void Logger::Log(LogLevel level, const std::string& msg) {
    tasks_.Push(LogTask(level, msg));
}

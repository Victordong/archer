#ifndef _ARCHER_LOGGER
#define _ARCHER_LOGGER

#include <iostream>
#include <map>
#include <thread>

#include "archer/base/noncopyable.hpp"
#include "archer/base/safe_queue.hpp"

namespace archer {
class Logger;

enum LogLevel { LOG_ERROR = 0, LOG_WARN, LOG_DEBUG, LOG_INFO };

static std::map<LogLevel, std::string> LogLevelMap{
    std::pair<LogLevel, std::string>(LogLevel::LOG_ERROR, "Error"),
    std::pair<LogLevel, std::string>(LogLevel::LOG_WARN, "Warn"),
    std::pair<LogLevel, std::string>(LogLevel::LOG_DEBUG, "Debug"),
    std::pair<LogLevel, std::string>(LogLevel::LOG_INFO, "Info")};

using LogTask = std::pair<LogLevel, std::string>;

using LogCallBack = std::function<void(const LogTask&)>;

class Logger : noncopyable {
   public:
    static Logger* GetLogger();

    void Log(LogLevel level, const std::string& msg);

    void Error(const std::string& msg);
    void Warn(const std::string& msg);
    void Debug(const std::string& msg);
    void Info(const std::string& msg);

    void set_log_cb(LogCallBack& cb) { log_cb_ = cb; };

    static void default_log(const LogTask& task) {
        std::cout << "Level[" << LogLevelMap[task.first] << "]" << task.second
                  << std::endl;
    }

   private:
    Logger(LogCallBack log_cb = default_log);
    inline static Logger* logger_ = nullptr;
    inline static std::mutex lock_;
    SafeQueue<LogTask> tasks_;
    LogCallBack log_cb_;
    std::thread log_thread_;
};

class WarnLogger : noncopyable {
   public:
    template <typename... Args>
    void operator()(const char* format, Args... args) {
        const int MAX_LOG_LENGTH = 1024;
        char s[MAX_LOG_LENGTH];
        sprintf(s, format, args...);
        Logger::GetLogger()->Warn(s);
    }
};

class ErrorLogger : noncopyable {
   public:
    template <typename... Args>
    void operator()(const char* format, Args... args) {
        const int MAX_LOG_LENGTH = 1024;
        char s[MAX_LOG_LENGTH];
        sprintf(s, format, args...);
        Logger::GetLogger()->Error(s);
    }
};

class InfoLogger : noncopyable {
   public:
    template <typename... Args>
    void operator()(const char* format, Args... args) {
        const int MAX_LOG_LENGTH = 1024;
        char s[MAX_LOG_LENGTH];
        sprintf(s, format, args...);
        Logger::GetLogger()->Info(s);
    }
};

class DebugLogger : noncopyable {
   public:
    template <typename... Args>
    void operator()(const char* format, Args... args) {
        const int MAX_LOG_LENGTH = 1024;
        char s[MAX_LOG_LENGTH];
        sprintf(s, format, args...);
        Logger::GetLogger()->Debug(s);
    }
};

inline WarnLogger WARN_LOG;
inline DebugLogger DEBUG_LOG;
inline InfoLogger INFO_LOG;
inline ErrorLogger ERR_LOG;

};  // namespace archer

#endif  // _ARCHER_LOGGER

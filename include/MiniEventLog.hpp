#pragma once
#include <iostream>
#include <string>
#include <mutex>

#define log_debug(fmt , ...)MiniEventLog::getInstance()->log(MiniEventLog::LOG_LEVEL_DEBUG, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define log_info(fmt , ...)MiniEventLog::getInstance()->log(MiniEventLog::LOG_LEVEL_INFO, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define log_warn(fmt , ...)MiniEventLog::getInstance()->log(MiniEventLog::LOG_LEVEL_WARN, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define log_error(fmt , ...)MiniEventLog::getInstance()->log(MiniEventLog::LOG_LEVEL_ERROR, __FILE__, __LINE__, fmt, ##__VA_ARGS__)


class MiniEventLog{

public:
    enum LogLevel {
        LOG_LEVEL_DEBUG,
        LOG_LEVEL_INFO,
        LOG_LEVEL_WARN,
        LOG_LEVEL_ERROR,
    };
public:
    static MiniEventLog* getInstance();
    void log(LogLevel level, const char* filename, int line, const char* fmt , ...);
private:
    MiniEventLog() = default;
    ~MiniEventLog() = default;
    MiniEventLog(const MiniEventLog&) = delete;
    MiniEventLog& operator=(const MiniEventLog&) = delete;
    
    std::mutex m_mutex;
};
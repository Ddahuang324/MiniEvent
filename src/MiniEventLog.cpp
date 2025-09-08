#include <mutex>
#include <chrono>
#include <cstdarg>
#include <iostream>
#include <iomanip>
#include <sstream>
#include "../include/MiniEventLog.hpp"


MiniEventLog* MiniEventLog::getInstance(){
    static MiniEventLog instance;
    return &instance;
 }

void MiniEventLog::log(LogLevel level, const char* filename, int line, const char* fmt , ...) { 
    std::lock_guard<std::mutex> lock(m_mutex);
    
    // 获取当前时间并格式化
    auto now = std::chrono::system_clock::now();
    auto current_time = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&current_time), "%Y-%m-%d %H:%M:%S");
    ss << "." << std::setfill('0') << std::setw(3) << ms.count();

    const char* level_str ;

    switch (level) {
     case LOG_LEVEL_DEBUG:
        level_str = "[DEBUG]";
        break;
    case LOG_LEVEL_INFO:
        level_str = "[INFO]";
        break;
    case LOG_LEVEL_WARN:
        level_str = "[WARN]";
        break;
    case LOG_LEVEL_ERROR:
        level_str = "[ERROR]";
        break;
    default:
        level_str = "[UNKNOW]";
        break;
    }
    
    char final_buf[2048] = {0};
    char content_buf[1024] = {0};

    va_list args;
    va_start(args, fmt);
    vsnprintf(content_buf, sizeof(content_buf)-1, fmt, args);
    va_end(args);
    snprintf(final_buf, sizeof(final_buf) - 1, "%s %s [%s:%d] %s\n", ss.str().c_str(), level_str, filename, line, content_buf);
    
    std::cout << final_buf;

 }

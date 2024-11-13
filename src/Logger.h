#ifndef LOGGER_H
#define LOGGER_H

#include <iostream>
#include <cstdarg>  // For va_list, va_start, va_end
#include <cstdio>

enum class LogLevel {
    DEBUG,
    INFO,
    WARNING,
    ERROR
};

class Logger {
public:

    Logger(LogLevel level) : currentLevel(level), enable_logging(true) {}


    void setLogLevel(LogLevel level) {
        currentLevel = level;
    }
    void log(LogLevel level, const char* format, ...) const {
        if (level >= currentLevel) {
            std::cout << getLevelString(level) << ": ";
            va_list args;
            va_start(args, format);
            vprintf(format, args);
            va_end(args);
            std::cout << std::endl;
        }
    }

    void debug(const char* format, ...) const {
        if (LogLevel::DEBUG >= currentLevel) {
            std::cout << "DEBUG: ";
            va_list args;
            va_start(args, format);
            vprintf(format, args);
            va_end(args);
        }
    }

    void info(const char* format, ...) const {
        if (LogLevel::INFO >= currentLevel) {
            std::cout << "INFO: ";
            va_list args;
            va_start(args, format);
            vprintf(format, args);
            va_end(args);
        }
    }

    void warning(const char* format, ...) const {
        if (LogLevel::WARNING >= currentLevel) {
            std::cout << "WARNING: ";
            va_list args;
            va_start(args, format);
            vprintf(format, args);
            va_end(args);
        }
    }

    void error(const char* format, ...) const {
        if (LogLevel::ERROR >= currentLevel) {
            std::cout << "ERROR: ";
            va_list args;
            va_start(args, format);
            vprintf(format, args);
            va_end(args);
        }
    }

private:
    LogLevel currentLevel;
    bool enable_logging;
    std::string getLevelString(LogLevel level) const {
        switch (level) {
            case LogLevel::DEBUG: return "DEBUG";
            case LogLevel::INFO: return "INFO";
            case LogLevel::WARNING: return "WARNING";
            case LogLevel::ERROR: return "ERROR";
            default: return "UNKNOWN";
        }
    }
};
#endif
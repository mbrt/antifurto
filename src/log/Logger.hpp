#pragma once

#include "LoggerSink.hpp"

#include <iostream>
#include <utility>

namespace antifurto {
namespace log {

/// This class provide exclusive write access to a LoggerSink until destruction.
class Logger
{
public:
    Logger(LoggerSink* sink, const char* name);
    ~Logger();
    
    Logger(const Logger& ) = delete;
    Logger& operator= (const Logger& ) = delete;
    
    Logger(Logger&& other) : sink_(other.sink_) { other.sink_ = nullptr; }
    Logger& operator= (Logger&& other) {
        std::swap(other.sink_, sink_);
        return *this;
    }
    
    template <typename T>
    Logger& operator<< (T&& v) {
        sink_->getStream() << std::forward<T>(v);
        return *this;
    }

private:
    LoggerSink* sink_;
};

}} // namespace antifurto::log

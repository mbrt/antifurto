#pragma once

#include "LoggerSink.hpp"
#include "NullStream.hpp"

#include <mutex>
#include <iostream>
#include <fstream>

namespace antifurto {
namespace log {

/// This is a no op sink
class NullSink : public LoggerSink
{
public:
    NullSink() {
        stream_.setstate(std::ios_base::failbit);
    }
    std::ostream& getStream() override {
        return stream_;
    }
    void lock() override { }
    void unlock() override { }
    bool isValid() const override {
        return false;
    }
    void close() override { }

private:
    onullstream stream_;
};

/// This is a valid sink, but it does not know its stream
class LockedSink : public LoggerSink
{
public:
    void lock() override {
        mutex_.lock();
    }
    void unlock() override {
        mutex_.unlock();
    }
    bool isValid() const override {
        return true;
    }

protected:
    std::mutex mutex_;
};

/// This sink writes to a file
class FileSink : public LockedSink
{
public:
    FileSink() { }
    FileSink(const char* file)
        : stream_(file) { }

    void setFile(const char* file) {
        stream_.open(file, std::ofstream::out | std::ofstream::app);
    }
    std::ostream& getStream() override {
        return stream_;
    }
    void close() override {
        stream_.close();
    }

private:
    std::ofstream stream_;
};

/// This sink writes to std::cerr
class CerrSink : public LockedSink
{
public:
    std::ostream& getStream() override {
        return std::cerr;
    }
    void close() { }
};

}} // namespace antifurto::log

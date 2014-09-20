#pragma once

#include <iosfwd>

namespace antifurto {
namespace log {

/// This interface abstracts a sink for logs.
/// A sink can be locked for exclusive access and provides a stream to use to
/// write logs.
class LoggerSink
{
public:
    virtual std::ostream& getStream() = 0;
    virtual void lock() = 0;
    virtual void unlock() = 0;
    virtual bool isValid() const = 0;
    virtual void close() = 0;
    virtual ~LoggerSink() {}
};

}} // namespace antifurto::log

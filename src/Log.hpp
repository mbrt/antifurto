#pragma once

#ifdef NDEBUG
#   define GOOGLE_STRIP_FILE_LINE
#endif
#include <glog/logging.h>

#define LOG_ERROR LOG(ERROR)
#define LOG_WARNING LOG(WARNING)
#define LOG_INFO LOG(INFO)
#define LOG_DEBUG VLOG(1)

namespace antifurto {

// fwd reference
class Configuration;

void initLogger(const Configuration& config);

} // namespace antifurto


// END OF OLD STUFF
//
#include <iosfwd>
#include <utility>

namespace antifurto {

// fwd reference
class Configuration;

namespace log {

class LoggerSink
{
public:
    virtual std::ostream& getStream();
    virtual void lock();
    virtual void unlock();
    virtual bool isValid() const;
    virtual const char* getName() const;
    virtual void setName(const char*);
    virtual ~LoggerSink() {}
};

class Logger
{
public:
    Logger(LoggerSink* sink);
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
        sink_->getStream() << std::forward(v);
        return *this;
    }

private:
    LoggerSink* sink_;
};

void init(const Configuration& config);
void reload();
Logger error();
Logger info();
Logger debug();

}} // namespace antifurto::log

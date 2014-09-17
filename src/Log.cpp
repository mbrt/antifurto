#include "Log.hpp"
#include "Config.hpp"

#include <boost/filesystem.hpp>
namespace bfs = boost::filesystem;

namespace antifurto {

void failureWriter(const char* data, int size)
{
    LOG_ERROR << std::string(data, data + size);
}

void initLogger(const Configuration& config)
{
    FLAGS_stderrthreshold = 0;
    FLAGS_colorlogtostderr = true;
    FLAGS_minloglevel = 0;
    if (config.log.level == Configuration::Log::Level::DEBUG)
        FLAGS_v = 1;
    FLAGS_max_log_size = 2; // 2MB
    FLAGS_stop_logging_if_full_disk = true;
    FLAGS_log_dir = config.log.dir;
    if (!bfs::exists(config.log.dir))
        bfs::create_directories(config.log.dir);
    google::InitGoogleLogging("antifurto");
    google::InstallFailureSignalHandler();
    google::InstallFailureWriter(&failureWriter);
}

} // namespace antifurto

// END OF OLD STUFF

#include <iostream>
#include <fstream>
#include <mutex>

namespace antifurto {
namespace log {
namespace { // anon

// see http://stackoverflow.com/questions/19200207/redirect-debug-output-to-null-stream-instead-of-stdcerr
template <class cT, class traits = std::char_traits<cT> >
class basic_nullbuf: public std::basic_streambuf<cT, traits> {
    typename traits::int_type overflow(typename traits::int_type c)
    {
        return traits::not_eof(c); // indicate success
    }
};

template <class cT, class traits = std::char_traits<cT> >
class basic_onullstream: public std::basic_ostream<cT, traits> {
public:
    basic_onullstream():
    std::basic_ios<cT, traits>(&m_sbuf),
    std::basic_ostream<cT, traits>(&m_sbuf)
    {
        this->init(&m_sbuf);
    }

private:
    basic_nullbuf<cT, traits> m_sbuf;
};

typedef basic_onullstream<char> onullstream;
typedef basic_onullstream<wchar_t> wonullstream;

constexpr int numLevels() {
    return 3;
}

// global state
std::mutex mutex;
std::ofstream fileStream;
onullstream nullStream;
// settings
std::string logFilePath;

/// This is a no op sink
class NullSink : public LoggerSink
{
    std::ostream& getStream() override {
        return nullStream;
    }
    void lock() override { }
    void unlock() override { }
    bool isValid() const {
        return false;
    }
    const char* getName() const override {
        return "";
    }
    void setName(const char*) override { }
};

/// This is a valid sink, but it does not know its stream
class ValidSink : public LoggerSink
{
    ValidSink(const char* name = "")
        : name_(name) { }
    
    void lock() override {
        mutex.lock();
    }
    void unlock() override {
        mutex.unlock();
    }
    bool isValid() const {
        return true;
    }
    const char* getName() const override {
        return name_;
    }
    void setName(const char* name) override {
        name_ = name;
    }
    
    const char* name_;
};

/// This sink writes to file
class FileSink : public ValidSink
{
    using ValidSink::ValidSink;
    
    std::ostream& getStream() override {
        return fileStream;
    }
};

/// This sink writes to std::cerr
class CerrSink : public ValidSink
{
    using ValidSink::ValidSink;
    
    std::ostream& getStream() override {
        return std::cerr;
    }
}


// unique sinks
NullSink nullSink;
std::array<CerrSink, numLevels()> defaultSinkVector {
    CerrSink{"ERROR"},
    CerrSink{"INFO"},
    CerrSink{"DEBUG"}
};


std::array<LoggerSink*, numLevels()> sinkVector {
    &defaultSinkVector[0],
    &defaultSinkVector[1],
    &defaultSinkVector[2]
};


Logger getLogger(int level)
{
    return Logger{sinkVector[level]};
}

} // anon namespace


Logger::Logger(LoggerSink* sink)
    : sink_(sink)
{
    if (sink_->isValid()) {
        sink_->lock();
        std::ostream& out = sink_->getStream();
        // dump date time
        // ...
        out << sink_->getName() << ": ";
    }
}

Logger::~Logger()
{
    if (sink_ && sink_->isValid()) {
        std::ostream& out = sink_->getStream();
        out << std::endl;
        out.flush();
        sink_->unlock();
    }
}


void init(const Configuration& config)
{
    // ...
    nullStream.setstate(std::ios_base::failbit);
}

void reload()
{
    std::unique_lock<std::mutex> lock(mutex);
    fileStream.close();
    fileStream.open(logFilePath.c_str());
}

Logger error()
{
    return getLogger(0);
}

Logger info()
{
    return getLogger(1);
}

Logger debug()
{
    return getLogger(2);
}

}} // namespace antifurto::log

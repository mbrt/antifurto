#include "Log.hpp"
#include "Logger.hpp"
#include "ConcreteLoggerSink.hpp"
#include "../Config.hpp"

#include <array>
#include <boost/filesystem.hpp>
namespace bfs = boost::filesystem;

namespace antifurto {
namespace log {
namespace { // anon

constexpr int numLevels() {
    return 4;
}

struct NamedSink
{
    LoggerSink* sink;
    const char* name;
};


// global instances
FileSink fileSink;
NullSink nullSink;
CerrSink cerrSink;
// global state
std::array<NamedSink, numLevels()> sinkVector;
std::string logFilePath;


Logger getLogger(int level)
{
    return Logger{sinkVector[level].sink, sinkVector[level].name};
}

/// used to default initialize the sinkVector
struct StaticInitializer
{
    StaticInitializer() {
        for (auto& s : sinkVector)
            s.sink = &cerrSink;
        sinkVector[0].name = "ERROR";
        sinkVector[1].name = "WARNING";
        sinkVector[2].name = "INFO";
        sinkVector[3].name = "DEBUG";
    }
};
StaticInitializer initializer;

} // anon namespace


void init(const Configuration& )
{
    // ... init sink vector and log dir if needed
}

void reload()
{
    try {
        std::unique_lock<FileSink> lock(fileSink);
        fileSink.close();
        fileSink.setFile(logFilePath.c_str());
    }
    catch (std::exception& e) {
        error() << "Cannot reload file log: " << e.what();
    }
}

Logger error()
{
    return getLogger(0);
}

Logger warning()
{
    return getLogger(1);
}

Logger info()
{
    return getLogger(2);
}

Logger debug()
{
    return getLogger(3);
}

}} // namespace antifurto::log

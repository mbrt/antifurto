#include "Logger.hpp"
#include "../text/ToString.hpp"
#include <chrono>

namespace antifurto {
namespace log {

Logger::Logger(LoggerSink* sink, const char* name)
    : sink_(sink)
{
    if (sink_->isValid()) {
        sink_->lock();
        std::ostream& out = sink_->getStream();
        out << text::toString(std::chrono::system_clock::now())
            << " - " << name << ": ";
    }
}

Logger::~Logger()
{
    if (sink_->isValid()) {
        std::ostream& out = sink_->getStream();
        out << std::endl;
        out.flush();
        sink_->unlock();
    }
}

}} // namespace antifurto::log

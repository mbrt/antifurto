#include "Log.hpp"
#include "StaticConfig.hpp"

#include <boost/filesystem.hpp>
#include <boost/log/utility/setup/file.hpp>

namespace antifurto {

using namespace boost;

void initLogger(const Configuration& config)
{
    filesystem::path logFile{ config::logDir() };
    logFile /= "antifurto_%Nlog";
    log::add_file_log(
        log::keywords::file_name = logFile.string(),
        log::keywords::rotation_size = 100 * 1024,      // rotate every 100K
        log::keywords::format = "[%TimeStamp%]: %Message%"
    );
    // determine log severity
    log::trivial::severity_level level;
    if (config.log.level == Configuration::Log::Level::DEBUG)
        level = log::trivial::debug;
    else
        level = log::trivial::info;
    log::core::get()->set_filter(
        log::trivial::severity >= level
    );
}

} // namespace antifurto

#include "Log.hpp"
#include "Config.hpp"

#include <boost/filesystem.hpp>
namespace fs = boost::filesystem;

namespace antifurto {

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
    if (!fs::exists(config.log.dir))
        fs::create_directories(config.log.dir);
    google::InitGoogleLogging("antifurto");
}

} // namespace antifurto

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

#pragma once

#include <iostream>
#include <glog/logging.h>

#define LOG_ERROR LOG(ERROR)
#define LOG_INFO LOG(INFO)
#define LOG_DEBUG VLOG(1)

namespace antifurto {

// fwd reference
class Configuration;

void initLogger(const Configuration& config);

} // namespace antifurto

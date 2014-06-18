#pragma once

#include <iostream>

#define LOG_ERROR std::cerr
#define LOG_INFO std::cerr
#define LOG_DEBUG std::cerr

namespace antifurto {

// fwd reference
class Configuration;

void initLogger(const Configuration& config);

} // namespace antifurto

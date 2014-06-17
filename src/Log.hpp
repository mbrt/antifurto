#pragma once

#include "Config.hpp"

#define BOOST_LOG_USE_COMPILER_TLS 0
#include <boost/log/trivial.hpp>

#define LOG_ERROR BOOST_LOG_TRIVIAL(error)
#define LOG_INFO BOOST_LOG_TRIVIAL(info)
#define LOG_DEBUG BOOST_LOG_TRIVIAL(debug)

namespace antifurto {

void initLogger(const Configuration& config);

} // namespace antifurto

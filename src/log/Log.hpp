#pragma once

#include "Logger.hpp"

#include <iosfwd>
#include <utility>

namespace antifurto {

// fwd reference
class Configuration;

namespace log {

void init(const Configuration& config);
void reload();
Logger error();
Logger warning();
Logger info();
Logger debug();

}} // namespace antifurto::log

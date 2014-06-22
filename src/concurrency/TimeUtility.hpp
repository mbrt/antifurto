#pragma once

#include <chrono>

namespace antifurto {
namespace concurrency {

/// Creates a time point given year, month, day and hour
std::chrono::system_clock::time_point
makeTimePoint (int year, int mon, int day,
               int hour, int min, int sec = 0);

/// Returns the time point representing tomorrow (at midnight)
std::chrono::system_clock::time_point tomorrow();

} // namespace concurrency
} // namespace antifurto

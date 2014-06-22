#include "TimeUtility.hpp"
#include "../Exception.hpp"

namespace antifurto {
namespace concurrency {

std::chrono::system_clock::time_point
makeTimePoint(int year, int mon, int day,
              int hour, int min, int sec)
{
    struct std::tm t;
    t.tm_sec = sec;        // second of minute (0 .. 59 and 60 for leap seconds)
    t.tm_min = min;        // minute of hour (0 .. 59)
    t.tm_hour = hour;      // hour of day (0 .. 23)
    t.tm_mday = day;       // day of month (0 .. 31)
    t.tm_mon = mon-1;      // month of year (0 .. 11)
    t.tm_year = year-1900; // year since 1900
    t.tm_isdst = -1;       // determine whether daylight saving time
    std::time_t tt = std::mktime(&t);
    if (tt == -1) {
        throw Exception("no valid system time");
    }
    return std::chrono::system_clock::from_time_t(tt);
}

std::chrono::system_clock::time_point tomorrow()
{
    using namespace std::chrono;
    auto now = system_clock::to_time_t(system_clock::now());
    std::tm now_local;
    localtime_r(&now, &now_local);
    return makeTimePoint(now_local.tm_year + 1900, now_local.tm_mon,
                         now_local.tm_mday + 1, 0, 0, 0);
}



} // namespace concurrency
} // namespace antifurto

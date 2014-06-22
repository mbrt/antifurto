#include "ToString.hpp"

#include <iomanip>

namespace antifurto {
namespace text {

// see http://stackoverflow.com/questions/17386790/fully-separated-date-with-milliseconds-from-stdchronosystem-clock
std::string toString(std::chrono::system_clock::time_point t,
                     ToStringFormat format, char dateSep, char dateTimeSep)
{
    std::time_t time = std::chrono::system_clock::to_time_t(t);
    auto rounded = std::chrono::system_clock::from_time_t(time);
    if (rounded > t) {
        --time;
        rounded -= std::chrono::seconds(1);
    }
    std::tm time_local;
    localtime_r(&time, &time_local);
    std::ostringstream out;
    out << time_local.tm_year + 1900 << dateSep
        << std::setfill('0') << std::setw(2)
        << time_local.tm_mon << dateSep
        << time_local.tm_mday;
    if (format <= ToStringFormat::SHORT)
        out << dateTimeSep << std::setfill('0') << std::setw(2)
            << time_local.tm_hour << ':'
            << time_local.tm_min << ':'
            << time_local.tm_sec;
    if (format == ToStringFormat::FULL)
        out << '.'
            << std::setfill('0') << std::setw(4)
            << std::chrono::duration_cast<
                  std::chrono::duration<int, std::milli>>(t - rounded).count();
    return out.str();
}


} // namespace text
} // namespace antifurto

#pragma once

#include <sstream>
#include <chrono>

namespace antifurto {
namespace text {
namespace detail {

inline std::string toStringImpl(std::ostringstream& out) {
    return out.str();
}

template <typename T, typename... Args>
std::string toStringImpl(std::ostringstream& out, T&& t, Args&&... as) {
    out << std::forward<T>(t);
    return toStringImpl(out, std::forward<Args>(as)...);
}

} // detail


template <typename... Args>
std::string toString(Args&&... as) {
    std::ostringstream out;
    return detail::toStringImpl(out, std::forward<Args>(as)...);
}

enum class ToStringFormat
{
    FULL,
    SHORT,
    DATE_ONLY
};

std::string toString(std::chrono::system_clock::time_point t,
                     ToStringFormat format = ToStringFormat::FULL,
                     char dateSep = '/',
                     char dateTimeSep = ' ');

} // namespace text
} // namespace antifurto

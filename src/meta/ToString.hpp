#pragma once

#include <sstream>

namespace antifurto {
namespace meta {
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

} // namespace meta
} // namespace antifurto

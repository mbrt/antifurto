#pragma once

#include <algorithm>
#include <iterator>

namespace antifurto {
namespace meta {

template <typename Container, typename Predicate>
void removeIf(Container& c, Predicate p)
{
    c.erase(
        std::remove_if(
            std::begin(c), std::end(c),
            std::move(p)),
        std::end(c));
}

} // namespace meta
} // namespace antifurto

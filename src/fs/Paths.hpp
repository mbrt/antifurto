#pragma once

#include <boost/filesystem.hpp>
#include <string>
#include <utility>

namespace antifurto {
namespace fs {
namespace detail {

template <typename... Args>
std::string concatPathsImpl(boost::filesystem::path& p)
{
    return p.string();
}

template <typename T, typename... Args>
std::string concatPathsImpl(boost::filesystem::path& p, T&& t, Args&&... args)
{
    p /= std::forward<T>(t);
    return concatPathsImpl(p, std::forward<Args>(args)...);
}

} // namespace detail

template <typename... Args>
std::string concatPaths(Args&&... args)
{
    boost::filesystem::path p;
    return detail::concatPathsImpl(p, std::forward<Args>(args)...);
}

} // namespace fs
} // namespace antifurto

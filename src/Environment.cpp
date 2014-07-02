#include "Environment.hpp"
#include "Exception.hpp"

#include <memory>
#include <boost/filesystem.hpp>
namespace fs = boost::filesystem;

#if defined(WIN32)
#   include <windows.h>
#elif defined(__linux__)
#   include <unistd.h>
#   include <sys/stat.h>
#   include <linux/limits.h>
#else
#   error "Unsupported platform"
#endif


namespace antifurto {

std::string getExecutableRuntimePath()
{
#if defined(WIN32)
    char szFileName[MAX_PATH];
    ::GetModuleFileNameA(::GetModuleHandle(NULL), szFileName, MAX_PATH);
    return szFileName;
#elif defined(__linux__)
    static const char* selfLink = "/proc/self/exe";
    std::array<char, PATH_MAX> linkName;
    ssize_t sz = readlink(selfLink, &linkName[0], PATH_MAX);
    if (sz < 0)
        throw Exception("Readlink on self failure");
    else if (sz > PATH_MAX)
        throw Exception("Path too long for process");
    linkName[sz] = '\0';
    return std::string{&linkName[0]};
#endif
}

std::string getExecutableDirectory()
{
    fs::path exePath{getExecutableRuntimePath()};
    return exePath.parent_path().string();
}

} // namespace antifurto

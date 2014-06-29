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
    struct stat sb;
    if (lstat(selfLink, &sb) == -1)
        throw Exception("Cannot stat self process");

    std::unique_ptr<char[]> linkName{new char[sb.st_size + 1]};
    ssize_t sz = readlink(selfLink, linkName.get(), sb.st_size + 1);
    if (sz < 0)
        throw Exception("Readlink on self failure");
    else if (sz > sb.st_size)
        throw Exception("Unexpected increase of self exe link size");
    linkName[sz] = '\0';
    return std::string{linkName.get()};
#endif
}

std::string getExecutableDirectory()
{
    fs::path exePath{getExecutableRuntimePath()};
    return exePath.parent_path().string();
}

} // namespace antifurto

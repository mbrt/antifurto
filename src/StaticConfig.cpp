#include "StaticConfig.hpp"
#include "Environment.hpp"
#include "fs/Paths.hpp"

namespace antifurto {
namespace config {

std::string exeDir()
{
    return getExecutableDirectory();
}

std::string configDir()
{
    return fs::concatPaths(exeDir(), "config");
}

std::string configFile()
{
    return fs::concatPaths(configDir(), "antifurto.cfg");
}

} // namespace config
} // namespace antifurto

#include "NamedPipe.hpp"
#include "Exception.hpp"
#include "../text/ToString.hpp"

#include <sys/types.h>
#include <sys/stat.h>
#include <boost/filesystem.hpp>
namespace bfs = boost::filesystem;

namespace antifurto {
namespace ipc {

NamedPipe::NamedPipe(std::string filename)
    : filename_(std::move(filename))
{
    if (::mkfifo(filename_.c_str(), S_IWUSR | S_IRUSR | S_IRGRP | S_IROTH) == -1)
        throw Exception(text::toString("Cannot create pipe. errno: ", errno));
}

NamedPipe::~NamedPipe()
{
    if (!filename_.empty() && bfs::exists(filename_))
        bfs::remove(filename_);
}

} // namespace ipc
} // namespace antifurto

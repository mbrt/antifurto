#pragma once

#include <string>

namespace antifurto {
namespace ipc {

/// This class allows to create a named pipe under Linux.
///
/// The constructor creates the pipe with the given filename, the distructor
/// removes it.
class NamedPipe
{
public:
    NamedPipe() {}
    NamedPipe(std::string filename);

    NamedPipe(NamedPipe&& other) noexcept
        : filename_(std::move(other.filename_))
    { other.filename_.clear(); }

    NamedPipe& operator =(NamedPipe&& other) noexcept {
        filename_ = std::move(other.filename_);
        other.filename_.clear();
        return *this;
    }

    ~NamedPipe();

    NamedPipe(const NamedPipe& ) = delete;
    NamedPipe& operator =(const NamedPipe& ) = delete;

    const std::string& getFilename() const {
        return filename_;
    }

private:
    std::string filename_;
};

} // namespace ipc
} // namespace antifurto

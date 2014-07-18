#pragma once

#include <string>

namespace antifurto {
namespace ipc {

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

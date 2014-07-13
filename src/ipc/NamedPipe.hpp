#pragma once

#include <string>

namespace antifurto {
namespace ipc {

class NamedPipe
{
public:
    NamedPipe() {}
    NamedPipe(std::string filename);
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

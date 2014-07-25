#pragma once

#include "Picture.hpp"
#include "concurrency/SpScQueue.hpp"
#include "ipc/NamedPipe.hpp"

#include <string>
#include <mutex>

namespace antifurto {

/// This is the controller for the live view
class LiveView
{
public:
    LiveView(const std::string& outFilenamePrefix, unsigned int num);
    void addPicture(const Picture& p);
    std::string getCurrentFilename() const;

private:
    void prepareOutDir(const std::string& outFilenamePrefix);
    void write(const Picture& p);

    using Filenames = std::vector<std::string>;
    using Pipes = std::vector<ipc::NamedPipe>;
    using Worker =
        concurrency::SpScQueue<Picture,
            std::function<void(const Picture&)>>;

    Filenames filenames_;
    Pipes pipes_;
    unsigned int currentIndex_ = 0;
    mutable std::mutex idxM_;
    Worker worker_;
};

} // namespace antifurto

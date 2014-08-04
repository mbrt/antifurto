#pragma once

#include "Picture.hpp"
#include "concurrency/SpScQueue.hpp"
#include "ipc/NamedPipe.hpp"

#include <string>
#include <mutex>
#include <atomic>

namespace antifurto {

/// This is the controller for the live view
class LiveView
{
public:
    LiveView(const std::string& outFilenamePrefix, unsigned int num);
    ~LiveView();

    bool addPicture(const Picture& p);
    std::string const& getCurrentFilename() const;

private:
    void prepareOutDir(const std::string& outFilenamePrefix);
    void write(const Picture& p);
    void consumeCurrentFileIfValid();

    using Filenames = std::vector<std::string>;
    using Pipes = std::vector<ipc::NamedPipe>;
    using Worker =
        concurrency::SpScQueue<Picture,
            std::function<void(const Picture&)>>;

    Filenames filenames_;
    Pipes pipes_;
    unsigned int currentIndex_ = 0;
    std::atomic<bool> stopping_;
    mutable std::mutex idxM_;
    Worker worker_;
};

} // namespace antifurto

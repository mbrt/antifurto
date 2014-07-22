#pragma once

#include "Picture.hpp"
#include "concurrency/SpScQueue.hpp"

#include <string>

namespace antifurto {

class LiveView
{
public:
    LiveView(const std::string& outFilenamePrefix, unsigned int num);
    void addPicture(const Picture& p);

private:
    void write(const Picture& p);

    using Filenames = std::vector<std::string>;
    using Worker =
        concurrency::SpScQueue<Picture,
            std::function<void(const Picture&)>>;

    Filenames filenames_;
    Worker worker_;
    unsigned int currentIndex_ = 0;
};

} // namespace antifurto

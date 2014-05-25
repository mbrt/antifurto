#pragma once

#include <string>
#include <list>
#include <chrono>

#include "Picture.hpp"

namespace antifurto {

/// This class saves pictures in jpeg format to a folder.
///
/// When the recording is started, the last N pictures are
/// saved, including all the pictures until recording is stopped.
class PictureArchive
{
public:
    PictureArchive(std::string folder, unsigned int recordBuffer = 5);

    void addPicture(Picture p);
    void startSaving();
    void stopSaving();

private:
    using Clock = std::chrono::system_clock::time_point;
    struct RecordItem {
        Clock time;
        Picture picture;

        RecordItem(Clock t, Picture&& p)
            : time(t), picture(std::move(p))
        { }
    };
    using RecordBuffer = std::list<RecordItem>;

    void enqueue(Picture&& p);
    void save(Picture const& p, Clock t);

    std::string folder_;
    unsigned int recordBufferSize_;
    RecordBuffer recordBuffer_;
    bool recording_;
};

} // namespace antifurto

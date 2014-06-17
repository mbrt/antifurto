#pragma once

#include <string>
#include <list>
#include <chrono>
#include <functional>

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

    using Observer = std::function<void(const std::string&)>;
    void addObserver(Observer o);
    void clearObservers();

private:
    using Clock = std::chrono::system_clock::time_point;
    struct RecordItem {
        Clock time;
        Picture picture;

        RecordItem(Clock t, Picture p)
            : time(t), picture(std::move(p))
        { }
    };
    using RecordBuffer = std::list<RecordItem>;
    using ObserverList = std::vector<Observer>;

    void enqueue(Picture p);
    void save(Picture& p, Clock t);
    void notifyObservers(const std::string& fileName);
    void prepareCurrentFolder();

    std::string baseFolder_;
    std::string currentFolder_;
    unsigned int recordBufferSize_;
    RecordBuffer recordBuffer_;
    bool recording_;
    ObserverList observers_;
};

} // namespace antifurto

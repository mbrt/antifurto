#pragma once

#include "PictureArchive.hpp"
#include "DropboxUploader.hpp"
#include "MotionDetector.hpp"

#include <boost/lockfree/spsc_queue.hpp>
#include <thread>
#include <string>
#include <mutex>
#include <condition_variable>
#include <memory>

namespace antifurto {

/// This class is responsible of the pictures recording during alarms
class RecordingController
{
public:
    RecordingController(MotionDetector& detector);
    ~RecordingController();
    void addPicture(Picture p);

private:
    void onAlarmStateChanged(MotionDetector::State state);
    void onPictureSaved(const std::string& fileName);
    void uploadWorker();
    void uploadFile(const std::string& fileName);

    using UploadQueue =
        boost::lockfree::spsc_queue<
            std::string,
            boost::lockfree::capacity<32768>>;

    PictureArchive archive_;
    std::unique_ptr<DropboxUploader> uploader_;
    std::thread uploaderThread_;
    bool uploadWorking_;
    UploadQueue uploadQueue_;
    std::mutex conditionMutex_;
    std::condition_variable uploadNeeded_;
};

} // namespace antifurto

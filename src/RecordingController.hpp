#pragma once

#include "PictureArchive.hpp"
#include "DropboxUploader.hpp"
#include "MotionDetector.hpp"
#include "Config.hpp"
#include "concurrency/SpScQueue.hpp"
#include "concurrency/fwd.hpp"

#include <string>
#include <memory>
#include <queue>
#include <mutex>

namespace antifurto {

/// This class is responsible of the pictures recording during alarms
class RecordingController
{
public:
    RecordingController(const Configuration& cfg,
                        MotionDetector& detector,
                        concurrency::TaskScheduler& scheduler);
    void addPicture(Picture p);
    void performMaintenance();

private:
    void initUploader(const Configuration& cfg);
    void onAlarmStateChanged(MotionDetector::State state);
    void onPictureSaved(const std::string& fileName);
    void uploadFile(const std::string& sourceFile);
    void deleteOlderPictures();
    void enqueueOlderPictures();

    using UploadWorker =
        concurrency::SpScQueue<std::string,
            std::function<void(const std::string&)>>;
    using RecordingWorker =
        concurrency::SpScQueue<Picture,
            std::function<void(Picture&)>>;
    using UploadQueue = std::queue<std::string>;

    Configuration::Recording config_;
    concurrency::TaskScheduler& scheduler_;
    PictureArchive archive_;
    DropboxUploader uploader_;
    UploadWorker uploadWorker_;
    RecordingWorker recordingWorker_;
    UploadQueue toUploadAfterQueue_;
    std::mutex toUploadAfterQueueMutex_;
};

} // namespace antifurto

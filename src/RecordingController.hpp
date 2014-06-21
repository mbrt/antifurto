#pragma once

#include "PictureArchive.hpp"
#include "DropboxUploader.hpp"
#include "MotionDetector.hpp"
#include "Config.hpp"
#include "concurrency/SpScQueue.hpp"

#include <string>
#include <memory>

namespace antifurto {

/// This class is responsible of the pictures recording during alarms
class RecordingController
{
public:
    RecordingController(const Configuration& cfg, MotionDetector& detector);
    void addPicture(Picture p);
    void performMaintenance();
    void deleteOlderPictures();

private:
    void onAlarmStateChanged(MotionDetector::State state);
    void onPictureSaved(const std::string& fileName);
    void uploadFile(const std::string& sourceFile);

    using UploadWorker =
        concurrency::SpScQueue<std::string,
            std::function<void(const std::string&)>>;
    using RecordingWorker =
        concurrency::SpScQueue<Picture,
            std::function<void(Picture&)>>;

    Configuration::Recording config_;
    PictureArchive archive_;
    std::unique_ptr<DropboxUploader> uploader_;
    UploadWorker uploadWorker_;
    RecordingWorker recordingWorker_;
};

} // namespace antifurto

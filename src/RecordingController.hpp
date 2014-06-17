#pragma once

#include "PictureArchive.hpp"
#include "DropboxUploader.hpp"
#include "MotionDetector.hpp"
#include "concurrency/SpScQueue.hpp"

#include <string>
#include <memory>

namespace antifurto {

/// This class is responsible of the pictures recording during alarms
class RecordingController
{
public:
    RecordingController(MotionDetector& detector);
    void addPicture(Picture p);

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

    PictureArchive archive_;
    std::unique_ptr<DropboxUploader> uploader_;
    UploadWorker uploadWorker_;
    RecordingWorker recordingWorker_;
};

} // namespace antifurto

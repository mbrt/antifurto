#include "RecordingController.hpp"
#include "Config.hpp"

namespace antifurto {

RecordingController::RecordingController(MotionDetector& detector)
    : archive_(config::archiveDir())
    , uploadWorker_([this](const std::string& f){ uploadFile(f); }, 150)
{
    detector.addObserver([this](MotionDetector::State s){
        onAlarmStateChanged(s);
    });
    try {
        uploader_.reset(new DropboxUploader(
                        "./", config::dropboxConfigFile()));
        if (uploader_->good()) {
            // register the callback to get file that are saved only if
            // upload works
            archive_.addObserver([this](std::string const& f){
                onPictureSaved(f);
            });
        }
    }
    catch (...) {
        // ignore uploader errors
    }
}

void RecordingController::addPicture(Picture p)
{
    archive_.addPicture(std::move(p));
}

void RecordingController::onAlarmStateChanged(MotionDetector::State state)
{
    using State = MotionDetector::State;
    switch (state) {
    case State::NO_ALARM:
        archive_.stopSaving();
        break;
    case State::ALARM:
        archive_.startSaving();
        break;
    case State::PRE_ALARM:
    default:
        break;
    }
}

void RecordingController::onPictureSaved(const std::string& fileName)
{
    uploadWorker_.enqueue(fileName);
}

void RecordingController::uploadFile(const std::string& fileName)
{
    uploader_->uploadFile(fileName, fileName);
}


} // namespace antifurto

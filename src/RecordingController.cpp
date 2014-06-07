#include "RecordingController.hpp"
#include "Config.hpp"

namespace antifurto {

RecordingController::RecordingController(MotionDetector& detector)
    : archive_(config::archiveDir())
    , uploader_("./", config::dropboxConfigFile())
    , uploadWorking_(false)
{
    detector.addObserver([this](MotionDetector::State s){
        onAlarmStateChanged(s);
    });
    if (uploader_.good()) {
        // register the callback to get file that are saved only if upload works
        archive_.addObserver([this](std::string const& f){
            onPictureSaved(f);
        });
        uploaderThread_ = std::thread([this]{ uploadWorker(); });
    }
}

RecordingController::~RecordingController()
{
    {
        std::unique_lock<std::mutex>(conditionMutex_);
        uploadWorking_ = false;
    }
    uploadNeeded_.notify_one();
    if (uploaderThread_.joinable())
        uploaderThread_.join();
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
    uploadQueue_.push(fileName);
    uploadNeeded_.notify_one();
}

void RecordingController::uploadWorker()
{
    uploadWorking_ = true;
    auto consumer = [this](std::string const& f) { uploadFile(f); };
    while (uploadWorking_) {
        uploadQueue_.consume_all(consumer);

        // wait on condition
        std::unique_lock<std::mutex>(conditionMutex_);
        uploadNeeded_.wait(conditionMutex_, [this, consumer]() {
           return !uploadWorking_ || uploadQueue_.consume_one(consumer);
        });
    }
}

void RecordingController::uploadFile(const std::string& fileName)
{
    uploader_.uploadFile(fileName, fileName);
}


} // namespace antifurto

#include "RecordingController.hpp"
#include "Config.hpp"
#include "StaticConfig.hpp"
#include "concurrency/TaskScheduler.hpp"
#include "concurrency/TimeUtility.hpp"
#include "log/Log.hpp"
#include "meta/Iterator.hpp"

#include <utility>
#include <algorithm>
#include <boost/filesystem.hpp>
namespace bfs = boost::filesystem;

namespace antifurto {
namespace {
    // this is used only to fake a scheduler for the minimal constructor
    concurrency::TaskScheduler staticScheduler;
}

RecordingController::RecordingController(const Configuration& cfg)
    : config_(cfg.recording)
    , scheduler_(staticScheduler)
    , currentState_(MotionDetector::State::NO_ALARM)
{ }

RecordingController::
RecordingController(const Configuration& cfg, MotionDetector& detector,
                    concurrency::TaskScheduler& scheduler)
    : config_(cfg.recording)
    , scheduler_(scheduler)
    , archive_(cfg.recording.archiveDir)
    , uploadWorker_([this](const std::string& f){ uploadFile(f); }, 150)
    , recordingWorker_([this](Picture& p){ archive_.addPicture(std::move(p)); }, 1024)
{
    detector.addObserver([this](MotionDetector::State s){
        onAlarmStateChanged(s);
    });
    initUploader(cfg);

    // schedule maintenance at every midnight
    using namespace std::chrono;
    auto maintenanceWork = [this] { performMaintenance(); };
    scheduler_.scheduleAt(concurrency::tomorrow() + minutes(1), [=] {
        performMaintenance();
        scheduler_.scheduleEvery(hours(24), maintenanceWork);
    });
}

void RecordingController::addPicture(Picture p)
{
    if (!recordingWorker_.enqueue(std::move(p)))
        log::error() << "Failed to save the picture, queue is full";
}

void RecordingController::performMaintenance()
{
    if (!bfs::exists(config_.archiveDir)) return;
    log::info() << "Performing archive maintenance";
    deleteOlderPictures();
    log::info() << "Maintenance completed";
}

void RecordingController::initUploader(const Configuration& cfg)
{
    try {
        uploader_ = configureDropboxUploader(cfg, config::exeDir());
        if (uploader_.good()) {
            // register the callback to get file that are saved only if
            // upload works
            archive_.addObserver([this](std::string const& f){
                onPictureSaved(f);
            });
        }
        else
            log::info() << "Failed initialization of Dropbox uploader";
    }
    catch (std::exception& e) {
        // ignore uploader errors
        log::info() << "Failed initialization of Dropbox uploader. "
                 << "Exception: " << e.what();
    }
}


void RecordingController::onAlarmStateChanged(MotionDetector::State state)
{
    using State = MotionDetector::State;
    currentState_ = state;
    switch (state) {
    case State::NO_MOTION:
        archive_.stopSaving();
        break;
    case State::NO_ALARM:
        enqueueOlderPicturesIfIdle();
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
    if (!uploadWorker_.enqueue(fileName)) {
        log::info() << "Failed to upload picture to Dropbox: queue is full";
        std::unique_lock<std::mutex> lock(toUploadAfterQueueMutex_);
        toUploadAfterQueue_.emplace(fileName);
    }
}

void RecordingController::uploadFile(const std::string& sourceFile)
{
    try {
        std::size_t archiveDirSize = config_.archiveDir.size();
        assert(sourceFile.size() > archiveDirSize);
        std::string dest{sourceFile.substr(archiveDirSize + 1)};
        uploader_.uploadFile(sourceFile, dest);
    }
    catch (std::exception& e) {
        log::error() << "Error uploading file: " << e.what();
    }
}

void RecordingController::deleteOlderPictures()
{
    unsigned int
            ndirs = std::distance(bfs::directory_iterator(config_.archiveDir),
                                  bfs::directory_iterator());
    int toDelete = ndirs - config_.maxDays;
    if (toDelete <= 0) return;

    log::info() << "Deleting " << toDelete << " archive days";
    std::vector<bfs::path> dirs;
    std::copy(bfs::directory_iterator(config_.archiveDir),
              bfs::directory_iterator(), std::back_inserter(dirs));
    std::sort(dirs.begin(), dirs.end());
    std::for_each(std::begin(dirs), std::begin(dirs) + toDelete,
                  [](bfs::path const& p) { bfs::remove_all(p); });
}

void RecordingController::enqueueOlderPicturesIfIdle()
{
    std::unique_lock<std::mutex> lock(toUploadAfterQueueMutex_);
    if (toUploadAfterQueue_.empty())
        return;
    if (currentState_ != MotionDetector::State::NO_ALARM) {
        log::info() << "Cannot upload now, not idle. Schedule a new upload";
        scheduleUploadOlderPictures();
    }

    log::info() << "Start uploading older pictures";
    while (!toUploadAfterQueue_.empty()) {
        if (uploadWorker_.enqueue(toUploadAfterQueue_.front()))
            toUploadAfterQueue_.pop();
        else
            break;
    }

    // if the queue is not empty, we need to schedule an upload after
    if (!toUploadAfterQueue_.empty()) {
        log::info() << "Cannot empty the upload queue. Schedule a new upload";
        scheduleUploadOlderPictures();
    }
    else
        log::info() << "Upload of older pictures completed";
}

void RecordingController::scheduleUploadOlderPictures()
{
    scheduler_.scheduleAfter(std::chrono::minutes(2), [this] {
        enqueueOlderPicturesIfIdle();
    });
}

} // namespace antifurto

#include "RecordingController.hpp"
#include "Config.hpp"
#include "StaticConfig.hpp"
#include "Log.hpp"
#include "concurrency/TaskScheduler.hpp"
#include "concurrency/TimeUtility.hpp"
#include "meta/Iterator.hpp"

#include <utility>
#include <algorithm>
#include <boost/filesystem.hpp>
namespace bfs = boost::filesystem;

namespace antifurto {

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
        LOG_ERROR << "Failed to enqueue the picture\n";
}

void RecordingController::performMaintenance()
{
    if (!bfs::exists(config_.archiveDir)) return;
    LOG_INFO << "Performing archive maintenance";
    deleteOlderPictures();
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
            LOG_INFO << "Failed initialization of Dropbox uploader";
    }
    catch (std::exception& e) {
        // ignore uploader errors
        LOG_INFO << "Failed initialization of Dropbox uploader. "
                 << "Exception: " << e.what();
    }
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

void RecordingController::uploadFile(const std::string& sourceFile)
{
    try {
        std::size_t archiveDirSize = strlen(config::archiveDir());
        assert(sourceFile.size() > archiveDirSize);
        std::string dest{sourceFile.substr(archiveDirSize + 1)};
        uploader_.uploadFile(sourceFile, dest);
    }
    catch (std::exception& e) {
        LOG_ERROR << "Error uploading file: " << e.what() << std::endl;
    }
}

void RecordingController::deleteOlderPictures()
{
    unsigned int
            ndirs = std::distance(bfs::directory_iterator(config_.archiveDir),
                                  bfs::directory_iterator());
    int toDelete = ndirs - config_.maxDays;
    if (toDelete <= 0) return;

    LOG_INFO << "Deleting " << toDelete << " archive days";
    std::vector<bfs::path> dirs;
    std::copy(bfs::directory_iterator(config_.archiveDir),
              bfs::directory_iterator(), std::back_inserter(dirs));
    std::sort(dirs.begin(), dirs.end());
    std::for_each(std::begin(dirs), std::begin(dirs) + toDelete,
                  [](bfs::path const& p) { bfs::remove_all(p); });
}

} // namespace antifurto

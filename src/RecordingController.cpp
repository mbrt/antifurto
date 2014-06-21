#include "RecordingController.hpp"
#include "Config.hpp"
#include "StaticConfig.hpp"
#include "Log.hpp"
#include "meta/Iterator.hpp"

#include <utility>
#include <algorithm>
#include <boost/filesystem.hpp>
namespace fs = boost::filesystem;

namespace antifurto {

RecordingController::
RecordingController(const Configuration& cfg, MotionDetector& detector)
    : config_(cfg.recording)
    , archive_(cfg.recording.archiveDir)
    , uploadWorker_([this](const std::string& f){ uploadFile(f); }, 150)
    , recordingWorker_([this](Picture& p){ archive_.addPicture(std::move(p)); }, 1024)
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
    if (!recordingWorker_.enqueue(std::move(p)))
        LOG_ERROR << "Failed to enqueue the picture\n";
}

void RecordingController::performMaintenance()
{
    if (!fs::exists(config_.archiveDir)) return;
    LOG_INFO << "Performing archive maintenance";
    deleteOlderPictures();
}

void RecordingController::deleteOlderPictures()
{
    unsigned int
            ndirs = std::distance(fs::directory_iterator(config_.archiveDir),
                                  fs::directory_iterator());
    int toDelete = ndirs - config_.maxDays;
    if (toDelete <= 0) return;

    LOG_INFO << "Deleting " << toDelete << " archive days";
    std::vector<fs::path> dirs;
    std::copy(fs::directory_iterator(config_.archiveDir),
              fs::directory_iterator(), std::back_inserter(dirs));
    std::sort(dirs.begin(), dirs.end());
    std::for_each(std::begin(dirs), std::begin(dirs) + toDelete,
                  [](fs::path const& p) { fs::remove_all(p); });
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
        uploader_->uploadFile(sourceFile, dest);
    }
    catch (std::exception& e) {
        LOG_ERROR << "Error uploading file: " << e.what() << std::endl;
    }
}


} // namespace antifurto

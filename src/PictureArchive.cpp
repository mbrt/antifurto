#include "PictureArchive.hpp"
#include "fs/Paths.hpp"
#include "text/ToString.hpp"

#include <sstream>
#include <ctime>
#include <chrono>
#include <iomanip>

#include <opencv2/highgui/highgui.hpp>
#include <boost/filesystem.hpp>
namespace bfs = boost::filesystem;

namespace antifurto {

PictureArchive::
PictureArchive(std::string folder, unsigned int recordBuffer)
    : baseFolder_(folder), recordBufferSize_(recordBuffer)
    , recording_(false)
{ }

void PictureArchive::addPicture(Picture p)
{
    if (recording_)
        save(p, std::chrono::system_clock::now());
    else
        enqueue(std::move(p));
}

void PictureArchive::startSaving()
{
    recording_ = true;
    prepareCurrentFolder();
    for (auto& i: recordBuffer_)
        save(i.picture, i.time);
    recordBuffer_.clear();
}

void PictureArchive::stopSaving()
{
    recording_ = false;
}

void PictureArchive::addObserver(PictureArchive::Observer o)
{
    observers_.emplace_back(std::move(o));
}

void PictureArchive::clearObservers()
{
    observers_.clear();
}

void PictureArchive::enqueue(Picture p)
{
    recordBuffer_.emplace_back(std::chrono::system_clock::now(), std::move(p));
    if (recordBuffer_.size() > recordBufferSize_)
        recordBuffer_.erase(recordBuffer_.begin());
}

void PictureArchive::save(Picture& p, Clock t)
{
    std::string filename{ fs::concatPaths(
        currentFolder_,
        text::toString(t, text::ToStringFormat::FULL, '-', '_') + ".jpg")};

    cv::putText(p, text::toString(t, text::ToStringFormat::SHORT, '/', ' '),
                cv::Point(30,30), CV_FONT_HERSHEY_COMPLEX_SMALL, 0.8,
                cv::Scalar(200,200,250), 1, CV_AA);
    cv::imwrite(filename, p, {CV_IMWRITE_JPEG_QUALITY, 90});
    notifyObservers(filename);
}

void PictureArchive::notifyObservers(const std::string& fileName)
{
    for (auto& obs: observers_)
        obs(fileName);
}

void PictureArchive::prepareCurrentFolder()
{
    currentFolder_ = fs::concatPaths(
        baseFolder_,
        text::toString(std::chrono::system_clock::now(),
                       text::ToStringFormat::DATE_ONLY, '-'));
    if (!bfs::exists(currentFolder_))
        bfs::create_directories(currentFolder_);
}

} // namespace antifurto

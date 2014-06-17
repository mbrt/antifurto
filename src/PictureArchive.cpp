#include "PictureArchive.hpp"
#include "meta/ToString.hpp"

#include <sstream>
#include <ctime>
#include <chrono>
#include <iomanip>

#include <opencv2/highgui/highgui.hpp>
#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;

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
    fs::path filename{currentFolder_};
    filename /= meta::toString(t, meta::ToStringFormat::FULL, '-', '_') + ".jpg";
    cv::putText(p, meta::toString(t, meta::ToStringFormat::SHORT, '/', ' '),
                cv::Point(30,30), CV_FONT_HERSHEY_COMPLEX_SMALL, 0.8,
                cv::Scalar(200,200,250), 1, CV_AA);
    cv::imwrite(filename.string(), p, { CV_IMWRITE_JPEG_QUALITY, 90 });
    notifyObservers(filename.string());
}

void PictureArchive::notifyObservers(const std::string& fileName)
{
    for (auto& obs: observers_)
        obs(fileName);
}

void PictureArchive::prepareCurrentFolder()
{
    fs::path current{baseFolder_};
    current /=  meta::toString(std::chrono::system_clock::now(),
                             meta::ToStringFormat::DATE_ONLY, '-');
    if (!fs::exists(current))
        fs::create_directories(current);
    currentFolder_ = current.string();
}

} // namespace antifurto

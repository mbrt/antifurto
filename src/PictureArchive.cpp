#include "PictureArchive.hpp"

#include <opencv2/highgui/highgui.hpp>
#include <sstream>
#include <ctime>
#include <chrono>
#include <iomanip>

namespace antifurto {

PictureArchive::
PictureArchive(std::string folder, unsigned int recordBuffer)
    : folder_(folder), recordBufferSize_(recordBuffer)
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
    for (auto const& i: recordBuffer_)
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

namespace { // anon

// see http://stackoverflow.com/questions/17386790/fully-separated-date-with-milliseconds-from-stdchronosystem-clock
std::string toStringTimePoint(std::chrono::system_clock::time_point t)
{
    std::time_t time = std::chrono::system_clock::to_time_t(t);
    auto rounded = std::chrono::system_clock::from_time_t(time);
    if (rounded > t) {
        --time;
        rounded -= std::chrono::seconds(1);
    }
    std::tm time_local;
    localtime_r(&time, &time_local);
    std::ostringstream out;
    out << time_local.tm_year + 1900 << '-'
        << std::setfill('0') << std::setw(2)
        << time_local.tm_mon << '-'
        << time_local.tm_mday << '_'
        << time_local.tm_hour << ':'
        << time_local.tm_min << ':'
        << time_local.tm_sec << '.'
        << std::setw(4)
        << std::chrono::duration_cast<
                std::chrono::duration<int, std::milli>>(t - rounded).count();
    return out.str();
}

} // anon namespace

void PictureArchive::save(const Picture& p, Clock t)
{
    std::stringstream filename;
    filename
        << folder_ << '/'
        << toStringTimePoint(t) << ".jpg";
    cv::imwrite(filename.str(), p, { CV_IMWRITE_JPEG_QUALITY, 90 });
    notifyObservers(filename.str());
}

void PictureArchive::notifyObservers(const std::string& fileName)
{
    for (auto& obs: observers_)
        obs(fileName);
}

} // namespace antifurto

#include "PictureArchive.hpp"

#include <opencv2/highgui/highgui.hpp>
#include <sstream>
#include <ctime>
#include <chrono>

namespace antifurto {

PictureArchive::
PictureArchive(std::string folder, unsigned int recordBuffer)
    : folder_(folder), recordBufferSize_(recordBuffer)
    , recording_(false)
{  }

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

void PictureArchive::enqueue(Picture&& p)
{
    recordBuffer_.emplace_back(std::chrono::system_clock::now(), std::move(p));
    if (recordBuffer_.size() > recordBufferSize_)
        recordBuffer_.erase(recordBuffer_.begin());
}

void PictureArchive::save(const Picture& p, Clock t)
{
    // see http://stackoverflow.com/questions/15957805/extract-year-month-day-etc-from-stdchronotime-point-in-c/15958113#15958113
    namespace chr = std::chrono;
    std::time_t time = chr::system_clock::to_time_t(t);
    char buffer[26];
    std::strftime(buffer, 26, "Y-m-d H:M:S", std::localtime(&time));

    std::stringstream filename;
    filename
        << folder_ << '/'
//        << std::put_time(std::localtime(&time), "Y-m-d H:M:S ")
        << ".jpg";
    //cv::imwrite()
}

} // namespace antifurto

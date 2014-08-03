#include "LiveView.hpp"
#include "Log.hpp"
#include "text/ToString.hpp"

#include <fcntl.h>
#include <opencv2/highgui/highgui.hpp>
#include <boost/filesystem.hpp>
namespace bfs = boost::filesystem;


namespace antifurto {

LiveView::LiveView(const std::string& outFilenamePrefix, unsigned int num)
    : worker_([&](const Picture& p){ write(p); }, num)
{
    prepareOutDir(outFilenamePrefix);
    for (unsigned int i = 0; i < num; ++i) {
        filenames_.emplace_back(text::toString(outFilenamePrefix, i, ".jpg"));
        std::string const& curr = filenames_.back();
        if (bfs::exists(curr))
            bfs::remove(curr);
        pipes_.emplace_back(curr);
    }
}

LiveView::~LiveView()
{
    for (const auto& fname : filenames_) {
        try {
            consumeFileIfValid(fname);
        }
        catch (...) { }
    }
}

bool LiveView::addPicture(const Picture& p)
{
    return worker_.enqueue(p);
}

std::string LiveView::getCurrentFilename() const
{
    std::lock_guard<std::mutex> lock(idxM_);
    return filenames_[currentIndex_];
}

void LiveView::prepareOutDir(const std::string& outFilenamePrefix)
{
    bfs::path prefixPath{outFilenamePrefix};
    bfs::path basePath = prefixPath.parent_path();
    if (!bfs::exists(basePath))
        bfs::create_directories(basePath);
}

void LiveView::write(const Picture& p)
{
    try {
        cv::imwrite(filenames_[currentIndex_], p, {CV_IMWRITE_JPEG_QUALITY, 90});
    }
    catch (std::exception& e) {
        LOG_ERROR << "Error saving image for live view: " << e.what();
    }

    std::lock_guard<std::mutex> lock(idxM_);
    currentIndex_ = (currentIndex_ + 1) % filenames_.size();
}

void LiveView::consumeFileIfValid(const std::string& fname)
{
    int fd = ::open(fname.c_str(), O_RDONLY | O_NONBLOCK);
    if (fd < 0) return;
    char buf[1024];
    int nread;
    while ((nread = ::read(fd, buf, sizeof(buf))) > 0) { }
    ::close(fd);
}

} // namespace antifurto

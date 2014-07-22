#include "LiveView.hpp"
#include "text/ToString.hpp"

#include <opencv2/highgui/highgui.hpp>


namespace antifurto {

LiveView::LiveView(const std::string& outFilenamePrefix, unsigned int num)
    : worker_([&](const Picture& p){ write(p); }, num)
{
    for (unsigned int i = 0; i < num; ++i)
        filenames_.emplace_back(text::toString(outFilenamePrefix, i));
}

void LiveView::addPicture(const Picture& p)
{
    worker_.enqueue(p);
}

void LiveView::write(const Picture& p)
{
    cv::imwrite(filenames_[currentIndex_++], p, {CV_IMWRITE_JPEG_QUALITY, 90});
}

} // namespace antifurto

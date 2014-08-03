#include "LiveViewController.hpp"
#include "LiveView.hpp"
#include "Config.hpp"
#include "Picture.hpp"

#include <fstream>
#include <iterator>

namespace antifurto {

LiveViewController::
LiveViewController(Configuration& c, PictureRegistrationRequest regF)
    : regFunc_(regF), timeout_(c.liveView.inactivityTimeout)
    , liveView_(new LiveView{c.liveView.filePrefix, c.liveView.numPictures})
{ }

LiveViewController::~LiveViewController()
{
    try {
        stop();
    }
    catch (...) { }
    stopWork_.get(); // wait the stop
}

void LiveViewController::addPicture(const Picture& p)
{
    using namespace std::chrono;

    if (liveView_->addPicture(p))
        lastPictureWrittenTime_ = system_clock::now();
    else if (system_clock::now() - lastPictureWrittenTime_ > timeout_)
        stop();
}

void LiveViewController::stop()
{
    if (!running_) return;
    running_ = false;

    regFunc_(false);
    stopWork_ = std::async(std::launch::async, [this] {
        auto token = std::async(std::launch::async, [this] {
            liveView_.reset();
        });
        if (token.wait_for(std::chrono::seconds(3)) != std::future_status::ready) {
            // consume the current picture
            std::ifstream f{liveView_->getCurrentFilename().c_str()};
            std::istream_iterator<uint8_t> it(f), end;
            while (it != end)
                ++it;
        }
        token.get();
    });
}

} // namespace antifurto

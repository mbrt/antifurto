#include "LiveViewController.hpp"
#include "LiveView.hpp"
#include "Config.hpp"
#include "Picture.hpp"
#include "log/Log.hpp"

#include <fstream>
#include <iterator>

namespace antifurto {

LiveViewController::
LiveViewController(const Configuration& c, PictureRegistrationRequest regF)
    : regFunc_(regF), socketPath_(c.liveView.socketAddress)
    , timeout_(c.liveView.inactivityTimeout)

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

void LiveViewController::start()
{
    if (running_) return;
    running_ = true;

    regFunc_(true);
    liveView_.reset(new LiveView{socketPath_});
}

void LiveViewController::stop()
{
    if (!running_) return;
    running_ = false;

    log::info() << "Stopping live view";
    stopWork_ = std::async(std::launch::async, [this] {
        regFunc_(false); // call this inside async to avoid deadlock
        liveView_.reset();
        log::info() << "Live view stopped";
    });
}

} // namespace antifurto

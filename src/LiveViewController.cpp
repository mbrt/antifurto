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
        stopWork_.get(); // wait the stop
    }
    catch (std::exception& e) {
        log::error() << "Error in stopping live view: " << e.what();
    }
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
    switch (status_) {
    case Status::STARTED:
        return;
    case Status::STOPPING:
        // wait for stop
        stopWork_.get();
        break;
    default:
        break;
    }

    // start the live view
    liveView_.reset(new LiveView{socketPath_});
    regFunc_(true);
    status_ = Status::STARTED;
}

void LiveViewController::stop()
{
    if (status_ != Status::STARTED) return;

    log::info() << "Stopping live view";
    status_ = Status::STOPPING;

    stopWork_ = std::async(std::launch::async, [this] {
        regFunc_(false);
        liveView_.reset();
        status_ = Status::STOPPED;
        log::info() << "Live view stopped";
    });
}

} // namespace antifurto

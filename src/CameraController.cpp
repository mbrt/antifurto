#include "CameraController.hpp"

namespace antifurto {

CameraController::CameraController()
    : metronome_(config::monitorCycleDuration())
    , thread_(&CameraController::controllerLoop, this)
{ }

CameraController::~CameraController()
{
    {
        std::lock_guard<std::mutex> lock(m_);
        done_ = true;
    }
    cv_.notify_one();
    thread_.join();
}

void CameraController::setPeriod(std::chrono::milliseconds period)
{
    metronome_.setCycleTime(period);
}

auto CameraController::
addObserver(Observer observer) -> Registration
{
    auto registration = subject_.registerObserver(std::move(observer));
    cv_.notify_one();
    return registration;
}

void CameraController::controllerLoop()
{
    Picture picture;
    while (true) {
        {
            std::unique_lock<std::mutex> lock(m_);
            while (!done_ && !subject_.hasObservers())
                cv_.wait(lock);
            if (done_) return;
        }

        do {
            metronome_.checkpoint();
            camera_.takePicture(picture);
        } while (subject_.notify(picture) && !done_);
    }
}

} // namespace antifurto

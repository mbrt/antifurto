#include "CameraController.hpp"

namespace antifurto {

CameraController::CameraController()
    : metronome_(config::monitorCycleDuration())
    , thread_(&CameraController::controllerLoop, this)
{ }

CameraController::~CameraController()
{
    {
        std::lock_guard<std::mutex> lock(cvm_);
        done_ = true;
    }
    cv_.notify_one();
    thread_.join();
}

namespace { // anon

template <typename List, typename Id>
auto findPeriodItem(List& list, Id id)
    -> decltype(std::begin(list))
{
    using Item = decltype(*std::begin(list));
    return std::find_if(std::begin(list),
                           std::end(list),
                           [=](const Item& p) {
        return p.id == id;
    });
}

} // anon namespace

void CameraController::
setDesiredPeriod(const Registration& r, Period period)
{
    std::lock_guard<std::mutex> lock(desiredPeriodsM_);
    auto it = findPeriodItem(desiredPeriods_, r.getId());
    if (it == std::end(desiredPeriods_)) return;
    it->period = period;
    updatePeriodFromDesired();
}

auto CameraController::
addObserver(Observer observer, Period desiredPeriod) -> Registration
{
    auto registration = subject_.registerObserver(std::move(observer));
    addToDesiredPeriods(registration.getId(), desiredPeriod);
    cv_.notify_one();
    return registration;
}

void CameraController::controllerLoop()
{
    Picture picture;
    while (true) {
        {
            std::unique_lock<std::mutex> lock(cvm_);
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

void CameraController::onObserverRemoved(Subject::Id id)
{
    std::lock_guard<std::mutex> lock(desiredPeriodsM_);
    removeFromDesiredPeriods(id);
    updatePeriodFromDesired();
}

void CameraController::addToDesiredPeriods(Subject::Id id, Period period)
{
    desiredPeriods_.emplace_back(period, id);
}

void CameraController::removeFromDesiredPeriods(Subject::Id id)
{
    auto it = findPeriodItem(desiredPeriods_, id);
    if (it == desiredPeriods_.end()) return;
    std::swap(*it, desiredPeriods_.back());
    desiredPeriods_.pop_back();
}

void CameraController::updatePeriodFromDesired()
{
    Period result = std::numeric_limits<Period>::max();
    for (auto& p : desiredPeriods_) {
        result = std::min(p.period, result);
    }
    metronome_.setCycleTime(result);
}

} // namespace antifurto

#include "MailNotificationController.hpp"
#include "Config.hpp"
#include "StaticConfig.hpp"

#include "log/Log.hpp"
#include "concurrency/TaskScheduler.hpp"

#include <iterator>


namespace antifurto {

MailNotificationController::
MailNotificationController(const Configuration& c,
                           MotionDetector& detector,
                           concurrency::TaskScheduler& scheduler)
    : contacts_(c.mail.destinations.begin(), c.mail.destinations.end())
    , scheduler_(scheduler)
    , lastNotificationTime_(std::chrono::system_clock::now() - config::minNotificationDelay())
{
    if (!contacts_.empty()) {
        detector.addObserver([this](MotionDetector::State s) {
            onAlarmStateChanged(s);
        });
    }
}

MailNotificationController::~MailNotificationController()
{
    // FIXME: It is better to kill the notifications here
    try {
        if (notificationWork_.valid())
            notificationWork_.get();
    }
    catch (...) { }
}

void MailNotificationController::
onAlarmStateChanged(MotionDetector::State state)
{
    switch (state) {
    case MotionDetector::State::ALARM:
        notifyContacts();
        break;
    case MotionDetector::State::NO_ALARM:
        processNotificationResults();
        break;
    default:
        break;
    }
}


void MailNotificationController::notifyContacts()
{
    // do not notify if already notified
    if (std::chrono::system_clock::now() - lastNotificationTime_
            < config::minNotificationDelay())
        return;
    // do not notify if previous notification not completed
    if (notificationWork_.valid()) {
        log::warning() << "Cannot notify via mail now: "
                          "last notification not completed";
        return;
    }

    // notify everything in another thread
    notificationWork_ =
        std::async(std::launch::async, [this] {
            notificationWork();
        });

    lastNotificationTime_ = std::chrono::system_clock::now();
}

void MailNotificationController::processNotificationResults()
{
    using namespace std::chrono;

    // check if notifications have to be checked
    if (!notificationWork_.valid()) return;

    try {
        if (notificationWork_.wait_for(milliseconds(0)) == std::future_status::ready)
            notificationWork_.get();
        else {
            log::warning() << "Mail notifications not completed. "
                              "New check scheduled";
            scheduler_.scheduleAfter(minutes(5), [this] {
                processNotificationResults();
            });
        }
    }
    catch (std::exception& e) {
        log::error() << "Error in notification work: " << e.what();
    }
}

void MailNotificationController::notificationWork()
{
    log::info() << "Notify via mail";

    try {
        notifier_.send(contacts_, "Antifurto alert",
                       "Alarm! motion detected!");
    }
    catch (std::exception& e) {
        log::error() << "Failed mail notification: " << e.what();
    }
}

} // namespace antifurto

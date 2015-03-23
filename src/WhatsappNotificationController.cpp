#include "WhatsappNotificationController.hpp"
#include "Config.hpp"
#include "StaticConfig.hpp"
#include "MotionDetector.hpp"
#include "concurrency/TaskScheduler.hpp"
#include "log/Log.hpp"
#include "meta/Algorithm.hpp"

namespace antifurto {

WhatsappNotificationController::
WhatsappNotificationController(const Configuration& c,
                               MotionDetector& detector,
                               concurrency::TaskScheduler& scheduler)
    : whatsapp_(configureWhatsappNotifier(c, config::exeDir()))
    , contacts_(c.whatsapp.destinations.begin(), c.whatsapp.destinations.end())
    , scheduler_(scheduler)
    , lastNotificationTime_(std::chrono::system_clock::now() - config::minNotificationDelay())
{
    if (!contacts_.empty()) {
        detector.addObserver([this](MotionDetector::State s) {
            onAlarmStateChanged(s);
        });
    }
}

WhatsappNotificationController::~WhatsappNotificationController()
{
    // FIXME: It is better to kill the notifications here
    try {
        if (notificationWork_.valid())
            notificationWork_.get();
    }
    catch (...) { }
}

void WhatsappNotificationController::
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

void WhatsappNotificationController::notifyContacts()
{
    // do not notify if already notified
    if (std::chrono::system_clock::now() - lastNotificationTime_
            < config::minNotificationDelay())
        return;
    // do not notify if previous notification not completed
    if (notificationWork_.valid()) {
        log::warning() << "Cannot notify via whatsapp now: "
                          "last notification not completed";
        return;
    }

    // notify everything in another thread
    notificationWork_ =
        std::async(std::launch::async, [this] {
            for (const auto& contact : contacts_)
                notifyContact(contact);
        });

    lastNotificationTime_ = std::chrono::system_clock::now();
}

void WhatsappNotificationController::notifyContact(const std::string& contact)
{
    log::info() << "Notify contact " << contact;
    try {
        whatsapp_.send(contact, "Alarm! Motion detected");
    }
    catch (std::exception& e) {
        log::error() << "Failed notification to " << contact
                     << ": " << e.what();
    }
}

void WhatsappNotificationController::processNotificationResults()
{
    using namespace std::chrono;

    // check if notifications have to be checked
    if (!notificationWork_.valid()) return;

    try {
        if (notificationWork_.wait_for(milliseconds(0)) == std::future_status::ready)
            notificationWork_.get();
        else {
            log::warning() << "Whatsapp notifications not completed. "
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

} // namespace antifurto

#include "NotificationController.hpp"
#include "Config.hpp"
#include "StaticConfig.hpp"
#include "Log.hpp"

#include <iostream>

namespace antifurto {

NotificationController::
NotificationController(const Configuration& c, MotionDetector& detector)
    : whatsapp_(configureWhatsappNotifier(c, config::exeDir()))
    , contacts_(c.whatsapp.destinations.begin(), c.whatsapp.destinations.end())
    , lastNotificationTime_(std::chrono::system_clock::now() - config::minNotificationDelay())
{
    if (!contacts_.empty()) {
        detector.addObserver([this](MotionDetector::State s) {
            onAlarmStateChanged(s);
        });
    }
}

NotificationController::~NotificationController()
{
    // FIXME: It is better to kill the notifications here
    for (auto& f : notifications_)
    {
        try {
            f.get();
        }
        catch (...) { }
    }
}

void NotificationController::onAlarmStateChanged(MotionDetector::State state)
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

void NotificationController::notifyContacts()
{
    // do not notify if already notified
    if (std::chrono::system_clock::now() - lastNotificationTime_
            < config::minNotificationDelay())
        return;

    for (const auto& contact : contacts_)
        notifications_.emplace_back(
            std::async(std::launch::async, [=] {
                // copy also whatsapp notifier, to avoid race conditions
                whatsapp_.send(contact, "Intrusion alarm!");
            }));

    lastNotificationTime_ = std::chrono::system_clock::now();
}

void NotificationController::processNotificationResults()
{
    for (auto& f : notifications_) {
        try {
            if (f.wait_for(std::chrono::milliseconds(500))
                    != std::future_status::ready)
                continue;

            f.get();
            notifications_.pop_front();
        }
        catch (std::exception& e) {
            LOG_ERROR << "Notification failed " << e.what() << std::endl;
        }
    }
    if (!notifications_.empty())
        LOG_ERROR << "Notifications not completed\n";
}

} // namespace antifurto

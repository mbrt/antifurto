#include "NotificationController.hpp"
#include "Config.hpp"

#include <iostream>

namespace antifurto {

NotificationController::
NotificationController(config::Configuration& c, MotionDetector& detector)
    : whatsapp_(".", config::whatsappConfigFile())
    , contacts_(c.whatsapp.destinations.begin(), c.whatsapp.destinations.end())
{
    if (!contacts_.empty()) {
        detector.addObserver([this](MotionDetector::State s){
            onAlarmStateChanged(s);
        });
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
    for (const auto& contact : contacts_)
        notifications_.emplace_back(
            std::async(std::launch::async, [=] {
                // copy also whatsapp notifier, to avoid race conditions
                whatsapp_.send(contact, "Intrusion alarm!");
            }));
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
            std::cerr << "Notification failed " << e.what() << std::endl;
        }
    }
    if (!notifications_.empty())
        std::cerr << "Notifications not completed\n";
}

} // namespace antifurto

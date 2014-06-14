#include "NotificationController.hpp"
#include "Config.hpp"

#include <future>
#include <iostream>

namespace antifurto {

NotificationController::
NotificationController(config::Configuration& c, MotionDetector& detector)
    : whatsapp_(".", config::whatsappConfigFile())
    , contacts_(c.whatsapp.destinations.begin(), c.whatsapp.destinations.end())
{
    detector.addObserver([this](MotionDetector::State s){
        onAlarmStateChanged(s);
    });
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
            std::async(std::launch::async, [this, contact] {
                whatsapp_.send(contact, "Intrusion alarm!");
            }));
}

void NotificationController::processNotificationResults()
{

}

} // namespace antifurto

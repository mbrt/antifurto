#include "NotificationController.hpp"
#include "Config.hpp"

#include <future>
#include <iostream>

namespace antifurto {

NotificationController::NotificationController(MotionDetector& detector)
    : whatsapp_(".", config::whatsappConfigFile())
{
    detector.addObserver([this](MotionDetector::State s){
        onAlarmStateChanged(s);
    });
}

void NotificationController::onAlarmStateChanged(MotionDetector::State state)
{
    if (state != MotionDetector::State::ALARM) return;
    std::async(std::launch::async, [this]{ notifyContacts(); });
}

void NotificationController::notifyContacts()
{
    for (const auto& contact : concats_)
    {
        try {
            whatsapp_.send(contact, "Intrusion alarm!");
        }
        catch (std::exception& e) {
            std::cerr << "Error sending to contact " << contact
                      << "\nException: " << e.what() << std::endl;
        }
    }
}


} // namespace antifurto

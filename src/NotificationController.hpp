#pragma once

#include "WhatsappNotifier.hpp"
#include "MotionDetector.hpp"

#include <vector>
#include <string>

namespace antifurto {

class NotificationController
{
public:
    NotificationController(MotionDetector& detector);
    void onAlarmStateChanged(MotionDetector::State state);

private:
    using ContactList = std::vector<std::string>;
    void notifyContacts();

    WhatsappNotifier whatsapp_;
    ContactList concats_;
};

} // namespace antifurto

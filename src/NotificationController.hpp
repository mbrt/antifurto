#pragma once

#include "WhatsappNotifier.hpp"
#include "MotionDetector.hpp"
#include "Config_fwd.hpp"

#include <vector>
#include <string>
#include <future>

namespace antifurto {

class NotificationController
{
public:
    NotificationController(config::Configuration& c, MotionDetector& detector);
    void onAlarmStateChanged(MotionDetector::State state);

private:
    using ContactList = std::vector<std::string>;
    void notifyContacts();

    WhatsappNotifier whatsapp_;
    ContactList contacts_;
};

} // namespace antifurto

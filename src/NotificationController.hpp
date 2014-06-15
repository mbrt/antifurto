#pragma once

#include "WhatsappNotifier.hpp"
#include "MotionDetector.hpp"
#include "Config_fwd.hpp"

#include <vector>
#include <deque>
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
    using Notifications = std::deque<std::future<void>>;

    void notifyContacts();
    void processNotificationResults();

    WhatsappNotifier whatsapp_;
    ContactList contacts_;
    Notifications notifications_;
};

} // namespace antifurto

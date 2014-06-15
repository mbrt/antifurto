#pragma once

#include "WhatsappNotifier.hpp"
#include "MotionDetector.hpp"

#include <vector>
#include <deque>
#include <string>
#include <future>

namespace antifurto {

// fwd reference
class Configuration;

class NotificationController
{
public:
    NotificationController(Configuration& c, MotionDetector& detector);
    ~NotificationController();

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

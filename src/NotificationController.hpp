#pragma once

#include "WhatsappNotifier.hpp"
#include "MotionDetector.hpp"

#include <vector>
#include <deque>
#include <string>
#include <future>
#include <chrono>

namespace antifurto {

// fwd reference
class Configuration;
namespace concurrency {
class TaskScheduler;
}

class NotificationController
{
public:
    NotificationController(const Configuration& c,
                           MotionDetector& detector,
                           concurrency::TaskScheduler& scheduler);
    ~NotificationController();

    void onAlarmStateChanged(MotionDetector::State state);

private:
    using ContactList = std::vector<std::string>;

    void notifyContacts();
    void notifyContact(const std::string& contact);
    void processNotificationResults();

    WhatsappNotifier whatsapp_;
    ContactList contacts_;
    concurrency::TaskScheduler& scheduler_;
    std::chrono::system_clock::time_point lastNotificationTime_;
    std::future<void> notificationWork_;
};

} // namespace antifurto

#pragma once

#include "MotionDetector.hpp"
#include "MailNotifier.hpp"

#include <chrono>
#include <future>


namespace antifurto {

// fwd reference
class Configuration;
namespace concurrency {
    class TaskScheduler;
}


/// This class handles the mail send to the registered users,
/// when an alarm fires.
class MailNotificationController
{
public:
    MailNotificationController(const Configuration& c,
                               MotionDetector& detector,
                               concurrency::TaskScheduler& scheduler);
    ~MailNotificationController();

    void onAlarmStateChanged(MotionDetector::State state);

private:
    void notifyContacts();
    void processNotificationResults();
    void notificationWork();

    MailNotifier notifier_;
    MailNotifier::ContactList contacts_;
    std::string senderMail_;
    concurrency::TaskScheduler& scheduler_;
    std::chrono::system_clock::time_point lastNotificationTime_;
    std::future<void> notificationWork_;
};

} // namespace antifurto

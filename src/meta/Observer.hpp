#pragma once

#include <vector>
#include <algorithm>
#include <mutex>
#include <atomic>


namespace antifurto {
namespace meta {
namespace detail {

template <typename Subject>
class SubjectRegistration
{
public:
    SubjectRegistration() : subject_(nullptr) { }

    SubjectRegistration(Subject& subject, typename Subject::Id id)
        : subject_(&subject), id_(id) { }

    SubjectRegistration(SubjectRegistration&& other) noexcept
        : subject_(other.subject_), id_(other.id_)
    { other.subject_ = nullptr; }

    SubjectRegistration& operator= (SubjectRegistration&& other) noexcept
    {
        std::swap(subject_, other.subject_);
        std::swap(id_, other.id_);
        return *this;
    }

    void clear() {
        if (subject_)
            subject_->unregisterObserver(id_);
        subject_ = nullptr;
    }

    typename Subject::Id getId() const {
        return id_;
    }

    SubjectRegistration(const SubjectRegistration& ) = delete;
    SubjectRegistration& operator =(const SubjectRegistration& ) = delete;

    ~SubjectRegistration() {
        try {
            clear();
        }
        catch (...) { }
    }

private:
    Subject* subject_;
    typename Subject::Id id_;
};

} // namespace detail


/// Observer subject class
template <typename... Params>
class Subject
{
public:
    using Observer = std::function<void(Params...)>;
    using Id = std::size_t;
    using Registration = detail::SubjectRegistration<Subject<Params...>>;
    using UnregistrationHandler = std::function<void(Id)>;

    // constructor needed to initialize nextFreeId_
    Subject() : nextFreeId_(1) { }

    /// Register an observer
    Registration registerObserver(Observer o) {
        std::unique_lock<std::mutex> lock{regM_, std::try_to_lock_t{}};
        if (lock.owns_lock()) {
            // we have the lock to the observer list, so register the observer
            // normally

            // we need to save the current state of nextFreeId to avoid race
            // conditions, because we have to use it twice
            Id currentId = nextFreeId_++;
            observers_.emplace_back(currentId, std::move(o));
            return { *this, currentId };
        }
        else {
            // we do not have the lock
            // this could mean we are now notifying observers, so save this
            // registration in the registration queue
            // the observer queue will be updated at the end of the notification
            std::lock_guard<std::mutex> qlock{regQueueM_};
            Id currentId = nextFreeId_++;
            registrationQueue_.emplace_back(Operation::REGISTRATION,
                                            currentId, std::move(o));
            return { *this, currentId };
        }
    }

    /// Unregister an observer
    /// @return true if the observer has been succesfully unregistered, false
    ///     if not found or the unregistration has been scheduled for being
    ///     completed asynchronously
    bool unregisterObserver(Id id) {
        std::unique_lock<std::mutex> lock{regM_, std::try_to_lock_t{}};
        if (lock.owns_lock()) {
            // we have the lock to the observer list, so unregister the
            // observer normally
            return unregisterObserverUnsafe(id);
        }
        else {
            // we do not have the lock
            // this could mean we are now notifying observers, so save this
            // unregistration in the registration queue
            // the observer queue will be updated at the end of the notification
            std::lock_guard<std::mutex> qlock{regQueueM_};
            registrationQueue_.emplace_back(Operation::UNREGISTRATION, id);
            return false;
        }
    }

    /// Notify to all observers
    /// @return true if some observer has been notified, false otherwise
    bool notify(Params... params) {
        std::lock_guard<std::mutex> lock(regM_);
        for (auto& item : observers_)
            item.observer(params...);
        bool result = !observers_.empty();
        handleRegistrationQueue();
        return result;
    }

    /// Tests whether observers are present
    bool hasObservers() const {
        std::lock_guard<std::mutex> lock(regM_);
        return !observers_.empty();
    }

    /// Set the handler that will be called whenever an observer unregister
    /// itself
    void setUnregistrationHandler(UnregistrationHandler h) {
        unregHandler_ = std::move(h);
    }

private:
    // this function unregisters an observer but it is NOT threadsafe
    bool unregisterObserverUnsafe(Id id) {
        auto it = observers_.begin();
        for (auto end = observers_.end(); it != end; ++it) {
            if (it->id == id)
                break;
        }
        if (it == observers_.end()) return false;
        std::swap(*it, observers_.back());
        observers_.pop_back();
        // call the handler if present
        if (unregHandler_)
            unregHandler_(id);
        return true;
    }

    // this function assumes that regM_ is owned by the current thread
    void handleRegistrationQueue() {
        std::lock_guard<std::mutex> lock{regM_};
        if (registrationQueue_.empty()) return;
        for (const auto& item : registrationQueue_) {
            if (item.type == Operation::REGISTRATION)
                observers_.emplace_back(item.id, std::move(item.observer));
            else
                unregisterObserverUnsafe(item.id);
        }
        registrationQueue_.clear();
    }

    struct ObserverItem {
        Id id;
        Observer observer;

        ObserverItem(Id id, Observer o)
            : id(id), observer(std::move(o))
        { }
    };

    enum class Operation {
        REGISTRATION,
        UNREGISTRATION
    };

    struct RegistrationItem {
        Operation type;
        Id id;
        Observer observer;

        RegistrationItem(Operation type, Id id, Observer o)
            : type(type), id(id), observer(std::move(o))
        { }

        RegistrationItem(Operation type, Id id)
            : type(type), id(id), observer()
        { }
    };

    using ObserverList = std::vector<ObserverItem>;
    using RegistrationList = std::vector<RegistrationItem>;

    ObserverList observers_;
    RegistrationList registrationQueue_;
    std::atomic<Id> nextFreeId_;
    UnregistrationHandler unregHandler_;
    mutable std::mutex regM_;
    std::mutex regQueueM_;
};

} // namespace meta
} // namespace antifurto

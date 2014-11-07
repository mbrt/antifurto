#pragma once

#include <vector>
#include <algorithm>
#include <mutex>


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

    /// Register an observer
    Registration registerObserver(Observer o) {
        std::size_t ignored;
        return registerObserver(std::move(o), ignored);
    }

    /// Register an observer and returns the number of observers
    Registration registerObserver(Observer o, std::size_t& numObservers) {
        std::lock_guard<std::mutex> lock(listM_);
        observers_.emplace_back(++currentId_, std::move(o));
        numObservers = observers_.size();
        return { *this, currentId_ };
    }

    /// Unregister an observer
    /// @return true if the observer has been succesfully unregistered
    bool unregisterObserver(Id id) {
        {
            std::lock_guard<std::mutex> lock(listM_);
            auto it = observers_.begin();
            for (auto end = observers_.end(); it != end; ++it) {
                if (it->id == id)
                    break;
            }
            if (it == observers_.end()) return false;
            std::swap(*it, observers_.back());
            observers_.pop_back();
        }
        if (unregHandler_)
            unregHandler_(id);
        return true;
    }

    /// Notify to all observers
    /// @return true if some observer has been notified, false otherwise
    bool notify(Params... params) {
        // WARNING: this could lead to deadlocks if observers tries to
        // register or unregister inside their callback
        std::lock_guard<std::mutex> lock(listM_);
        for (auto& item : observers_)
            item.observer(params...);
        return !observers_.empty();
    }

    bool hasObservers() const {
        std::lock_guard<std::mutex> lock(listM_);
        return !observers_.empty();
    }

    /// Set the handler that will be called whenever an observer unregister
    /// itself
    void setUnregistrationHandler(UnregistrationHandler h) {
        unregHandler_ = std::move(h);
    }

private:
    struct ObserverItem {
        Id id;
        Observer observer;

        ObserverItem(Id id, Observer o)
            : id(id), observer(std::move(o))
        { }
    };

    using ObserverList = std::vector<ObserverItem>;
    ObserverList observers_;
    Id currentId_ = 0;
    UnregistrationHandler unregHandler_;
    mutable std::mutex listM_;
};

} // namespace meta
} // namespace antifurto

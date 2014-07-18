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

    /// Register an observer
    Registration registerObserver(Observer o) {
        std::lock_guard<std::mutex> lock(listM_);
        observers_.emplace_back(nextFreeId_, std::move(o));
        return { *this, nextFreeId_++ };
    }

    /// Unregister an observer
    bool unregisterObserver(Id id) {
        std::lock_guard<std::mutex> lock(listM_);
        auto it = observers_.begin();
        for (auto end = observers_.end(); it != end; ++it) {
            if (it->id == id)
                break;
        }
        if (it == observers_.end()) return false;
        std::swap(*it, observers_.back());
        observers_.pop_back();
        return true;
    }

    /// Notify to all observers
    void notify(Params&&... params) {
        // WARNING: this could lead to deadlocks if observers tries to
        // register or unregister inside their callback
        std::lock_guard<std::mutex> lock(listM_);
        for (auto& item : observers_)
            // no forward here, because if params is an rvalue and observer
            // register itself with a function that takes rvalues, the first
            // call move the param into the observer, and the second... gets a
            // moved out value
            item.observer(params...);
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
    Id nextFreeId_ = 0;
    std::mutex listM_;
};

} // namespace meta
} // namespace antifurto

#pragma once

#include <vector>
#include <algorithm>
#include <mutex>


namespace antifurto {
namespace meta {
namespace detail {

template <typename Subject>
class RegistrationToken
{
public:
    RegistrationToken() : subject_(nullptr) { }

    RegistrationToken(Subject& subject, typename Subject::Id id)
        : subject_(&subject), id_(id) { }

    RegistrationToken(RegistrationToken&& other) noexcept
        : subject_(other.subject_), id_(other.id_)
    { other.subject_ = nullptr; }

    RegistrationToken& operator= (RegistrationToken&& other) noexcept
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

    RegistrationToken(const RegistrationToken& ) = delete;
    RegistrationToken& operator =(const RegistrationToken& ) = delete;

    ~RegistrationToken() {
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
    using Registration = detail::RegistrationToken<Subject<Params...>>;

    /// Register an observer
    Registration registerObserver(Observer o) {
        std::lock_guard<std::mutex> lock(listM_);
        observers_.emplace_back(nextFreeId_, std::move(o));
        return { *this, nextFreeId_++ };
    }

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
            item.observer(std::forward<Params>(params)...);
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

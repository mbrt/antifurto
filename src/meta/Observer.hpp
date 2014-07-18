#pragma once

#include <vector>

namespace antifurto {
namespace meta {

// we need this otherwise we cannot explicitly specialise for the funtion
// signature later
template <typename Signature>
class Subject;

/// Observer subject class
template <typename Return, typename... Params>
class Subject<Return(Params...)>
{
public:
    using Observer = std::function<Return(Params...)>;

    /// Register an observer
    void registerObserver(Observer o) {
        observers_.emplace_back(std::move(o));
    }

    /// Notify to all observers
    void notify(Params&&... params) {
        for (auto& obs : observers_)
            obs(std::forward<Params>(params)...);
    }

private:
    using ObserverList = std::vector<Observer>;
    ObserverList observers_;
};

} // namespace meta
} // namespace antifurto

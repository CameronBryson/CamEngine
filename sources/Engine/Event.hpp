#pragma once
#include <functional>
#include <unordered_map>
#include <vector>

template<typename T>
class EventDispatcher {
public:
    using EventCallback = std::function<void()>;

    void AddListener(T event, const EventCallback& callback) {
        listeners[event].push_back(callback);
    }

    void RemoveListener(T event) {
        listeners.erase(event);
    }

    void SendEvent(T event) {
        auto it = listeners.find(event);
        if (it != listeners.end()) {
            for (const auto& callback : it->second) {
                callback();
            }
        }
    }

private:
    std::unordered_map<T, std::vector<EventCallback>> listeners;
};
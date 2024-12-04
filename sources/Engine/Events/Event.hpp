#pragma once
#include <functional>
#include <map>
#include <unordered_map>
#include <vector>
#include <string>
#include <mutex>
template <typename T>
	class Event
{
protected:
	T m_Type;
	std::string m_Name;
	bool m_Handled = false;
public:
	Event() = default;
	Event(T type, const std::string& name = "") : m_Type(type), m_Name(name) {}
	virtual ~Event() {}
	inline const T GetType() const { return m_Type; }

	template<typename EventType>
	inline const EventType& ToType() const
	{
		return static_cast<const EventType&>(*this);
	}

	inline const std::string& GetName() const { return m_Name; }
	virtual bool Handled() const { return m_Handled; }
};
template<typename T>
class EventDispatcher
{
private:
	using Func = std::function<void(const Event<T>&)>;
	using ListenerHandle = std::shared_ptr<Func>;

	std::map<T, std::vector<ListenerHandle>> m_Listeners;
	int m_NextListenerID = 0;
	std::map<int, std::pair<T, ListenerHandle>> m_ListenerHandles;
	std::mutex m_Mutex;

public:
	int AddListener(T type, const Func& func)
	{
		std::lock_guard<std::mutex> lock(m_Mutex);
		auto listener = std::make_shared<Func>(func);
		m_Listeners[type].push_back(listener);
		int handle = m_NextListenerID++;
		m_ListenerHandles[handle] = {type, listener};
		return handle;
	}

	void RemoveListener(int handle)
	{
		std::lock_guard<std::mutex> lock(m_Mutex);
		auto it = m_ListenerHandles.find(handle);
		if (it != m_ListenerHandles.end())
		{
			const auto& listenerInfo = it->second;
			auto& listeners = m_Listeners[listenerInfo.first];
			auto listenerIt = std::find(listeners.begin(), listeners.end(), listenerInfo.second);
			if (listenerIt != listeners.end())
			{
				listeners.erase(listenerIt);
			}
			m_ListenerHandles.erase(it);
		}
	}

	void SendEvent(const Event<T>& event)
	{
		std::lock_guard<std::mutex> lock(m_Mutex);
		if (m_Listeners.find(event.GetType()) == m_Listeners.end())
			return; // Return if no listener is there for this event.

		// Loop through all listeners. If the event is not handled yet, we continue to process it.
		for (const auto& listener : m_Listeners.at(event.GetType()))
		{
			if (!event.Handled())
				(*listener)(event);
		}
	}
};
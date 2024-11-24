#pragma once
#include <functional>
#include <map>
#include <unordered_map>
#include <vector>
#include <string>
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
	std::map<T, std::vector<Func>> m_Listeners;
	int m_NextListenerID = 0;
	std::map<int, std::pair<T, typename std::vector<Func>::iterator>> m_ListenerHandles;
public:
	int AddListener(T type, const Func& func)
	{
		m_Listeners[type].push_back(func);
		int handle = m_NextListenerID++;
		m_ListenerHandles[handle] = { type, std::prev(m_Listeners[type].end()) };
		return handle;
	}
	void RemoveListener(int handle)
	{
		auto it = m_ListenerHandles.find(handle);
		if (it != m_ListenerHandles.end()) {
			const auto& listenerInfo = it->second;
			auto& listeners = m_Listeners[listenerInfo.first];
			listeners.erase(listenerInfo.second);
			m_ListenerHandles.erase(it);
		}
	}
	void SendEvent(const Event<T>& event)
	{
		if (m_Listeners.find(event.GetType()) == m_Listeners.end())
			return; // Return if no Listner is there for this event.

		// Loop though all Listeners. If the event is not handled yet, we continue to process it.
		for (auto&& listener : m_Listeners.at(event.GetType())) {
			if (!event.Handled()) listener(event);
		}
	}
};
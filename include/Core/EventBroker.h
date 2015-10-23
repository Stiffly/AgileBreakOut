/*
	This file is part of Daydream Engine.
	Copyright 2014 Adam Byléhn, Tobias Dahl, Simon Holmberg, Viktor Ljung
	
	Daydream Engine is free software: you can redistribute it and/or modify
	it under the terms of the GNU Lesser General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.
	
	Daydream Engine is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU Lesser General Public License for more details.
	
	You should have received a copy of the GNU Lesser General Public License
	along with Daydream Engine.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef MessageRelay_h__
#define MessageRelay_h__

#include <typeinfo>
#include <functional>
#include <map>
#include <unordered_map>
#include <list>
#include <tuple>

#define EVENT_SUBSCRIBE_MEMBER(relay, handler) \
	relay = decltype(relay)(std::bind(handler, this, std::placeholders::_1)); \
	EventBroker->Subscribe(relay);

namespace dd
{

struct Event
{
protected:
	Event() { }
};

class EventBroker;

class BaseEventRelay
{
friend class EventBroker;

protected:
	BaseEventRelay(std::string contextTypeName, std::string eventTypeName)
		: m_ContextTypeName(contextTypeName)
		, m_EventTypeName(eventTypeName)
		, m_Broker(nullptr) { }
	~BaseEventRelay();

public:
	virtual bool Receive(const std::shared_ptr<Event> event) = 0;

protected:
	std::string m_ContextTypeName;
	std::string m_EventTypeName;
	EventBroker* m_Broker;
};

template <typename ContextType, typename EventType>
class EventRelay : public BaseEventRelay
{
public:
	typedef std::function<bool(const EventType&)> CallbackType;

	EventRelay()
		: m_Callback(nullptr)
		, BaseEventRelay(typeid(ContextType).name(), typeid(EventType).name()) { }
	EventRelay(CallbackType callback) 
		: m_Callback(callback)
		, BaseEventRelay(typeid(ContextType).name(), typeid(EventType).name()) { }

protected:
	bool Receive(const std::shared_ptr<Event> event) override;

private:
	CallbackType m_Callback;
};

template <typename ContextType, typename EventType>
bool EventRelay<ContextType, EventType>::Receive(const std::shared_ptr<Event> event)
{
	if (m_Callback != nullptr)
	{
		return m_Callback(*static_cast<const EventType*>(event.get()));
	}
	else
	{
		return false;
	}
}

class EventBroker
{
template <typename ContextType, typename EventType> friend class EventRelay;

public:
	EventBroker()
	{
		m_EventQueueRead = std::make_shared<EventQueue_t>();
		m_EventQueueWrite = std::make_shared<EventQueue_t>();
	}

	void Subscribe(BaseEventRelay &relay);
	template <typename EventType>
	void Publish(const EventType &event);
	// Process all events no matter the context.
	/*void Process()
	{

	}*/
	/*
		Process all events in a given context.
		Returns: Number of events processed
	*/
	template <typename ContextType>
	int Process();
	int Process(std::string contextTypeName);
	void Swap();
	void Clear();
	void Unsubscribe(BaseEventRelay &relay);

private:
	bool m_IsProcessing = false;

	typedef std::string ContextTypeName_t; // typeid(ContextType).name()
	typedef std::string EventTypeName_t; // typeid(EventType).name()

	typedef std::unordered_multimap<EventTypeName_t, BaseEventRelay*> EventRelays_t;
	typedef std::unordered_map<ContextTypeName_t, EventRelays_t> ContextRelays_t;
	ContextRelays_t m_ContextRelays;
	std::vector<BaseEventRelay*> m_RelaysToSubscribe;
	std::vector<BaseEventRelay*> m_RelaysToUnsubscribe;

	typedef std::list<std::pair<EventTypeName_t, std::shared_ptr<Event>>> EventQueue_t;
	std::shared_ptr<EventQueue_t> m_EventQueueRead;
	std::shared_ptr<EventQueue_t> m_EventQueueWrite;

	void subscribeImmediate(BaseEventRelay& relay);
	void unsubscribeImmediate(BaseEventRelay& relay);
};

template <typename EventType>
void EventBroker::Publish(const EventType &event)
{
	/*auto itpair = m_Subscribers.equal_range(typeid(EventType).name());
	for (auto it = itpair.first; it != itpair.second; ++it)
	{
		it->second->Receive(event);
	}*/

	m_EventQueueWrite->push_back(std::make_pair(typeid(EventType).name(), std::shared_ptr<EventType>(new EventType(event))));
}

template <typename ContextType>
int EventBroker::Process()
{
	const std::string contextTypeName = typeid(ContextType).name();
	return Process(contextTypeName);
}

}

#endif // MessageRelay_h__

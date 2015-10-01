#ifndef EVENTFIXTURE_H
#define EVENTFIXTURE_H

#include <boost/test/unit_test.hpp>
#include "Core/EventBroker.h"

template <typename EventType>
struct EventFixture
{
	EventFixture()
	{
		this->EventBroker = new dd::EventBroker();
		m_EEventType = decltype(m_EEventType)(std::bind(&OnEvent, this, std::placeholders::_1));
		this->EventBroker->Subscribe(m_EEventType);
		Run();
		Check();
	}
	~EventFixture()
	{
		this->EventBroker->Unsubscribe(m_EEventType);
		delete this->EventBroker;
	}

	dd::EventBroker* EventBroker = nullptr;
	dd::EventRelay<EventFixture, EventType> m_EEventType;
	bool m_EventRecieved = false;
	EventType Before;
	EventType After;

	bool OnEvent(const EventType& event)
	{
		m_EventRecieved = true;
		After = event;

		return true;
	}

	void Run()
	{
		// Publish the event
		this->EventBroker->Publish(Before);
		// Clear to swap buffers
		this->EventBroker->Swap();
		// Process the event
		this->EventBroker->template Process<EventFixture>();
	}

	void Check()
	{
		BOOST_CHECK(m_EventRecieved);
	}
};

#endif

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

#include "PrecompiledHeader.h"
#include "Core/EventBroker.h"

dd::BaseEventRelay::~BaseEventRelay()
{
	if (m_Broker != nullptr)
	{
		m_Broker->Unsubscribe(*this);
	}
}

void dd::EventBroker::Unsubscribe(BaseEventRelay &relay) // ?
{
	if (m_IsProcessing) {
		m_RelaysToUnsubscribe.push_back(&relay);
	} else {
		unsubscribeImmediate(relay);
	}
}

void dd::EventBroker::Subscribe(BaseEventRelay &relay)
{
	if (m_IsProcessing) {
		m_RelaysToSubscribe.push_back(&relay);
	} else {
		subscribeImmediate(relay);
	}
}

int dd::EventBroker::Process(std::string contextTypeName)
{
	m_IsProcessing = true;

	auto it = m_ContextRelays.find(contextTypeName);
	if (it == m_ContextRelays.end())
		return 0;

	EventRelays_t &relays = it->second;

	int eventsProcessed = 0;
	for (auto &pair : *m_EventQueueRead)
	{
		std::string &eventTypeName = pair.first;
		std::shared_ptr<Event> event = pair.second;

		auto itpair = relays.equal_range(eventTypeName);
		for (auto it2 = itpair.first; it2 != itpair.second; it2++)
		{
			std::string name = it2->first;
			BaseEventRelay* relay = it2->second;
			relay->Receive(event);
			eventsProcessed++;
		}
	}

	m_IsProcessing = false;

	// Process pending subscriptions
	for (auto& r : m_RelaysToSubscribe) {
		subscribeImmediate(*r);
	}
	m_RelaysToSubscribe.clear();

	// Process pending unsubscriptions
	for (auto& r : m_RelaysToUnsubscribe) {
		unsubscribeImmediate(*r);
	}
	m_RelaysToUnsubscribe.clear();

	return eventsProcessed;
}

void dd::EventBroker::Swap()
{
	std::swap(m_EventQueueRead, m_EventQueueWrite);
}

void dd::EventBroker::Clear()
{
	m_EventQueueWrite->clear();
}

void dd::EventBroker::subscribeImmediate(dd::BaseEventRelay& relay)
{
	relay.m_Broker = this;
	m_ContextRelays[relay.m_ContextTypeName].insert(std::make_pair(relay.m_EventTypeName, &relay));
}

void dd::EventBroker::unsubscribeImmediate(dd::BaseEventRelay& relay)
{
	auto contextIt = m_ContextRelays.find(relay.m_ContextTypeName);
	if (contextIt == m_ContextRelays.end()) {
		return;
	}

	auto eventRelays = contextIt->second;
	auto itpair = eventRelays.equal_range(relay.m_EventTypeName);
	for (auto it = itpair.first; it != itpair.second; ++it) {
		if (it->second == &relay) {
			relay.m_Broker = nullptr;
			eventRelays.erase(it);
			break;
		}
	}
}

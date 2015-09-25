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
	auto contextIt = m_ContextRelays.find(relay.m_ContextTypeName);
	if (contextIt == m_ContextRelays.end())
		return;

	auto eventRelays = contextIt->second;

	auto itpair = eventRelays.equal_range(relay.m_EventTypeName);
	for (auto it = itpair.first; it != itpair.second; ++it)
	{
		if (it->second == &relay)
		{
			relay.m_Broker = nullptr;
			eventRelays.erase(it);
			break;
		}
	}
}

void dd::EventBroker::Subscribe(BaseEventRelay &relay)
{
	relay.m_Broker = this;
	m_ContextRelays[relay.m_ContextTypeName].insert(std::make_pair(relay.m_EventTypeName, &relay));
}

int dd::EventBroker::Process(std::string contextTypeName)
{
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
		for (auto it2 = itpair.first; it2 != itpair.second; ++it2)
		{
			auto relay = it2->second;
			relay->Receive(event);
			eventsProcessed++;
		}
	}

	return eventsProcessed;
}

void dd::EventBroker::Swap()
{
	std::swap(m_EventQueueRead, m_EventQueueWrite);
	m_EventQueueWrite->clear();
}

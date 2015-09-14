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

#ifndef InputController_h__
#define InputController_h__

#include <memory>

#include "EventBroker.h"
#include "EMouseMove.h"
#include "Input/EInputCommand.h"

namespace dd
{

template <typename EventContext>
class InputController
{
public:
	InputController(std::shared_ptr<dd::EventBroker> eventBroker)
		: EventBroker(eventBroker)
	{ Initialize(); }

	virtual void Initialize()
	{
		EVENT_SUBSCRIBE_MEMBER(m_EInputCommand, &InputController::OnCommand);
		EVENT_SUBSCRIBE_MEMBER(m_EMouseMove, &InputController::OnMouseMove);
	}

	virtual bool OnCommand(const Events::InputCommand &event) { return false; }
	virtual bool OnMouseMove(const Events::MouseMove &event) { return false; }

protected:
	std::shared_ptr<dd::EventBroker> EventBroker;

private:
	EventRelay<EventContext, Events::InputCommand> m_EInputCommand;
	EventRelay<EventContext, Events::MouseMove> m_EMouseMove;
};

}

#endif // InputController_h__

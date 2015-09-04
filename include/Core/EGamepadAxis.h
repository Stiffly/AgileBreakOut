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

#ifndef Events_GamepadAxis_h__
#define Events_GamepadAxis_h__

#include "Core/EventBroker.h"

namespace dd
{

namespace Gamepad
{
	/** Gamepad axis type */
	enum class Axis
	{
		LeftX,
		LeftY,
		RightX,
		RightY,
		LeftTrigger,
		RightTrigger,
		LAST = RightTrigger
	};
}

namespace Events
{

/** Thrown when a gamepad axis changes value */
struct GamepadAxis : Event
{
	/** ID of the gamepad. */
	int GamepadID;
	/** The axis type. */
	Gamepad::Axis Axis;
	/** The new value of the axis. */
	float Value;
};

}

}

#endif // Events_GamepadAxis_h__
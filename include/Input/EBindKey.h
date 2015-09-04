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

#ifndef Events_BindKey_h__
#define Events_BindKey_h__

#include "Core/EventBroker.h"

namespace dd
{

namespace Events
{

/** Called to bind a keyboard key to an input command. */
struct BindKey : Event
{
	/** The GLFW key code to bind. */
	int KeyCode;
	/** The command to send. */
	std::string Command;
	/** The value to send for positive stimulation.

		Multiplied by the 0-1 clamped value of the key.
	*/
	float Value;
};

}

}

#endif // Events_BindKey_h__
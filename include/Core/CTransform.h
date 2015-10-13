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

#ifndef Components_Transform_h__
#define Components_Transform_h__

#include "Core/Component.h"

namespace dd
{

namespace Components
{

/** Spatial data of an entity. */
struct Transform : public Component
{
	Transform()
		: Scale(glm::vec3(1.f)) { }

	/** Position in world coordinates (meters). */
	glm::vec3 Position;
	/** Orientation in quaternion form. */
	glm::quat Orientation;
	/** Current velocity of the entity in m/s. */
	glm::vec3 Velocity;
	/** Physical scale multiplier. */
	glm::vec3 Scale;

	// Sticky is if it sticks with the player or not. True means it will follow the camera.
	bool Sticky = true;
};

}

}

#endif // Components_Transform_h__

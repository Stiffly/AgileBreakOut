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
#include "Transform/TransformSystem.h"
#include "Core/World.h"

void dd::Systems::TransformSystem::Initialize()
{
}

glm::vec3 dd::Systems::TransformSystem::AbsolutePosition(EntityID entity)
{
	glm::vec3 absPosition;
	glm::quat accumulativeOrientation;

	do
	{
		auto transform = m_World->GetComponent<Components::Transform>(entity);
		//absPosition += transform->Position;
		entity = m_World->GetEntityParent(entity);
		auto transform2 = m_World->GetComponent<Components::Transform>(entity);
		if (entity == 0)
			absPosition += transform->Position;
		else
			absPosition = transform2->Orientation * (absPosition + transform->Position);
	} while (entity != 0);

	return absPosition * accumulativeOrientation;
}

glm::quat dd::Systems::TransformSystem::AbsoluteOrientation(EntityID entity)
{
	glm::quat absOrientation;

	do
	{
		auto transform = m_World->GetComponent<Components::Transform>(entity);
		absOrientation = transform->Orientation * absOrientation;
		entity = m_World->GetEntityParent(entity);
	} while (entity != 0);

	return absOrientation;
}

glm::vec3 dd::Systems::TransformSystem::AbsoluteScale(EntityID entity)
{
	glm::vec3 absScale(1);

	do
	{
		auto transform = m_World->GetComponent<Components::Transform>(entity);
		absScale *= transform->Scale;
		entity = m_World->GetEntityParent(entity);
	} while (entity != 0);

	return absScale;
}

dd::Components::Transform dd::Systems::TransformSystem::AbsoluteTransform(EntityID entity)
{
	glm::vec3 absPosition;
	glm::quat absOrientation;
	glm::vec3 absScale(1);

	do
	{
		auto transform = m_World->GetComponent<Components::Transform>(entity);
		entity = m_World->GetEntityParent(entity);
		auto transform2 = m_World->GetComponent<Components::Transform>(entity);

		// Position
		if (entity == 0)
			absPosition += transform->Position;
		else
			absPosition = transform2->Orientation * (absPosition + transform->Position);
		// Orientation
		absOrientation = transform->Orientation * absOrientation;
		// Scale
		absScale *= transform->Scale;
	} while (entity != 0);

	Components::Transform transform;
	transform.Position = absPosition;
	transform.Orientation = absOrientation;
	transform.Scale = absScale;

	return transform;
}


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
	EVENT_SUBSCRIBE_MEMBER(m_EMove, &Systems::TransformSystem::OnMove);
	EVENT_SUBSCRIBE_MEMBER(m_ERotate, &Systems::TransformSystem::OnRotate);
}

void dd::Systems::TransformSystem::Update(double dt)
{

	return;
}

void dd::Systems::TransformSystem::UpdateEntity(double dt, EntityID entity, EntityID parent)
{
	if (m_MoveItems.find(entity) != m_MoveItems.end())
	{
		auto transform = m_World->GetComponent<Components::Transform>(entity);

		glm::vec3 direction = glm::normalize(m_MoveItems[entity].GoalPosition - transform->Position);
		glm::vec3 movement = direction * m_MoveItems[entity].Speed * (float)dt;

		glm::vec3 v1 = m_MoveItems[entity].GoalPosition - transform->Position;
		glm::vec3 v2 = movement;

		if (glm::length(v2) >= glm::length(v1))
		{
			transform->Position = m_MoveItems[entity].GoalPosition;
			//transform->Velocity = glm::vec3(0, 0, 0);

			m_MoveItems.erase(entity);

			if (m_QueuedMoveItems.find(entity) != m_QueuedMoveItems.end())
			{
				auto it = m_QueuedMoveItems.find(entity);
				m_MoveItems[entity] = it->second;
				m_QueuedMoveItems.erase(it->first);
			}
		}
		else
		{
			transform->Position += movement;
			//transform->Velocity = movement;
		}
	}

	if (m_RotationItems.find(entity) != m_RotationItems.end())
	{
		auto transform = m_World->GetComponent<Components::Transform>(entity);
		auto absolutetransform = AbsoluteTransform(entity);

		float percentage = dt / m_RotationItems[entity].Time;
		m_RotationItems[entity].Time -= dt;
		glm::clamp(percentage, 0.f, 1.0f);
		transform->Orientation = glm::slerp(transform->Orientation, m_RotationItems[entity].GoalRotation, percentage);

		if (m_RotationItems[entity].Time <= 0)
		{
			m_RotationItems.erase(entity);

			if (m_QueuedRotationItems.find(entity) != m_QueuedRotationItems.end())
			{
				auto it = m_QueuedRotationItems.find(entity);
				m_RotationItems[entity] = it->second;
				m_QueuedRotationItems.erase(it->first);
			}
		}
	}
}

bool dd::Systems::TransformSystem::OnRotate(const Events::Rotate &event)
{
	RotationItems item;
	item.Entity = event.Entity;
	item.GoalRotation = event.GoalRotation;
	item.Time = event.Time;

	if (event.Queue)
	{
		if (m_RotationItems.find(event.Entity) != m_RotationItems.end())
		{
			m_QueuedRotationItems.insert(std::make_pair(event.Entity, item));
		}
		else
		{
			m_RotationItems[event.Entity] = item;
		}
	}
	else
	{
		m_RotationItems[event.Entity] = item;
	}
	return true;
}

bool dd::Systems::TransformSystem::OnMove(const Events::Move &event)
{
	MoveItems item;
	item.Entity = event.Entity;
	item.GoalPosition = event.GoalPosition;
	item.Speed = event.Speed;

	if (event.Queue)
	{
		if (m_MoveItems.find(event.Entity) != m_MoveItems.end())
		{
			m_QueuedMoveItems.insert(std::make_pair(event.Entity, item));
		}
		else
		{
			m_MoveItems[event.Entity] = item;
		}
	}
	else
	{
		m_MoveItems[event.Entity] = item;
	}
	return true;
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


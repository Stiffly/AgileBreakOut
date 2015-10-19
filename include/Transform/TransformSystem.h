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

#ifndef TransformSystem_h__
#define TransformSystem_h__

#include <unordered_map>

#include "Core/System.h"
#include "Core/CTransform.h"

#include "EMove.h"
#include "ERotate.h"

namespace dd
{

namespace Systems
{

class TransformSystem : public System
{
public:
	TransformSystem(World* world, std::shared_ptr<dd::EventBroker> eventBroker)
		: System(world, eventBroker)
	{ }

	void Initialize() override;
	void Update(double dt) override;
	void UpdateEntity(double dt, EntityID entity, EntityID parent) override;

	Components::Transform AbsoluteTransform(EntityID entity);
	glm::vec3 AbsolutePosition(EntityID entity);
	glm::quat AbsoluteOrientation(EntityID entity);
	glm::vec3 AbsoluteScale(EntityID entity);

	// Events
	EventRelay<TransformSystem, Events::Move> m_EMove;
	bool OnMove(const Events::Move &event);
	EventRelay<TransformSystem, Events::Rotate> m_ERotate;
	bool OnRotate(const Events::Rotate &event);
private:
	struct MoveItems
	{
		EntityID Entity;
		glm::vec3 GoalPosition;
		float Speed;
	};
	std::unordered_map<EntityID, MoveItems> m_MoveItems;
	std::multimap<EntityID, MoveItems> m_QueuedMoveItems;
	struct RotationItems
	{
		EntityID Entity;
		glm::quat GoalRotation;
		double Time;
	};
	std::unordered_map<EntityID, RotationItems> m_RotationItems;
	std::multimap<EntityID, RotationItems> m_QueuedRotationItems;
};

}

}

#endif // TransformSystem_h__

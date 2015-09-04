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

#ifndef System_h__
#define System_h__

#include "Util/Factory.h"
#include "Entity.h"
#include "Component.h"
#include "EventBroker.h"
#include "ResourceManager.h"

namespace dd
{

class World;

class System
{
public:
	System(World* world, std::shared_ptr<EventBroker> eventBroker)
		: m_World(world)
		, EventBroker(eventBroker)
	{ }
	virtual ~System() { }

	virtual void RegisterComponents(ComponentFactory* cf) { }
	virtual void RegisterResourceTypes(std::shared_ptr<dd::ResourceManager> rm) { }

	virtual void Initialize() { }

	// Called once per system every tick
	virtual void Update(double dt) { }
	// Called once for every entity in the world every tick
	virtual void UpdateEntity(double dt, EntityID entity, EntityID parent) { }

	// Called when a component is removed
	virtual void OnComponentRemoved(EntityID entity, std::string type, Component* component) { }
	// Called when components are committed to an entity
	virtual void OnEntityCommit(EntityID entity) { }
	virtual void OnEntityRemoved(EntityID entity) { }

protected:
	World* m_World;
	std::shared_ptr<dd::EventBroker> EventBroker;
};

class SystemFactory : public Factory<System> { };

}

#endif // System_h__
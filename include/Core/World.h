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

#ifndef World_h__
#define World_h__

#include <stack>
#include <map>
#include <set>
#include <unordered_map>
#include <vector>
#include <string>
#include <queue>

#include <boost/any.hpp>

#include "Util/Factory.h"
#include "Entity.h"
#include "Component.h"
#include "System.h"
#include "EventBroker.h"
#include "EComponentCreated.h"
#include "ResourceManager.h"

namespace dd
{

/** The entity manager.

	Governs over the lifetime of entities.
*/
class World
{
public:
	World(std::shared_ptr<dd::EventBroker> eventBroker)
		: EventBroker(eventBroker)
		, m_LastEntityID(0) { }
	~World() { }

	/** Initialize the world.

		
	*/
	virtual void Initialize();

	/** Register world systems.

		Called by Initialize.
		Implemented to register systems with the systemfactory before 
		adding them to the world.
	*/
	virtual void RegisterSystems() {}
	/** Add systems to the world.
	
		Called by Initialize.
		Implemented to fill the world with the desired systems.
	*/ 
	virtual void AddSystems() {}
	/** Register components with the world.
		
		Implemented to register World-global components.
		@deprecated Components are now registered through individual Systems.
	*/
	// TODO: Get rid of this
	virtual void RegisterComponents() {}

	/** Add a system to the World */
	template <typename T>
	void AddSystem()
	{
		m_Systems[typeid(T).name()] = std::shared_ptr<System>(SystemFactory.Create<T>());
	}
	/** Fetch a system by type.		

		@tparam T The type of the system to fetch.
	*/
	template <typename T>
	std::shared_ptr<T> GetSystem();

	/** Creates an empty entity.
	
		@param parent The parent of the entity.
	*/
	EntityID CreateEntity(EntityID parent = 0);
	/** Recursively clones an entity and all of its components and children.

		@param entity The entity to clone.
		@param parent The new parent of the clone.
	*/
	EntityID CloneEntity(EntityID entity, EntityID parent = 0);
	/** Removes an entity and all its children from the world. */
	void RemoveEntity(EntityID entity);
	/** Check if an entity is valid and exists in the world.

		Returns FALSE if the entity doesn't exist or is in queue to be removed.
		@returns Whether the entity exists or not.
	*/
	bool ValidEntity(EntityID entity);
	/** Get the parent of an entity. */
	EntityID GetEntityParent(EntityID entity);
	/** Get the top level parent of an entity.
	
		Gets the top level parent of an entity. Loops through the scene 
		graph to find the first entity with the world as its parent.
	*/
	EntityID GetEntityBaseParent(EntityID entity);
	/** Get the children of an entity.
	
		@returns A list of entities parented to this entity.
	*/
	std::list<EntityID> GetEntityChildren(EntityID entity);
	/** Set the parent of an entity.
	
		@param entity The entity to reparent.
		@param newParent The new parent of the entity.
	*/
	void SetEntityParent(EntityID entity, EntityID newParent);

	/** Get a property field of an entity by name.
	
		@tparam T The type of the property.
		@param entity The entity.
		@param property The name of the property to fetch.
	*/
	template <class T>
	T GetProperty(EntityID entity, std::string property)
	{
		if(m_EntityProperties.find(entity) == m_EntityProperties.end())
			return T();
		if(m_EntityProperties[entity].find(property) == m_EntityProperties[entity].end())
			return T();

		return boost::any_cast<T>(m_EntityProperties[entity][property]);
	}
	/** Set a property field of an entity.
	
		@param entity The entity.
		@param property The name of the property to set.
		@param value The value of the property.
	*/
	void SetProperty(EntityID entity, std::string property, boost::any value)
	{
		m_EntityProperties[entity][property] = value;
	}
	/** Set a string property field of an entity.

		Helper function to be able to set string properties easier.
		@param entity The entity.
		@param property The name of the property to set.
		@param value The value of the property.
	*/
	void SetProperty(EntityID entity, std::string property, char* value)
	{
		m_EntityProperties[entity][property] = std::string(value);
	}

	/** Create and attach component to an entity.
	
		@tparam T The type of component to attach.
		@param entity The entity to attach the component to.
		@returns The newly attached component.
	*/
	template <class T>
	std::shared_ptr<T> AddComponent(EntityID entity);
	/** Remove a component from an entity.
	
		@tparam T The type of the component to remove.
		@param entity The entity to remove the component from.
	*/
	template <class T>
	void RemoveComponent(EntityID entity);
	/** Fetch a component of an entity.
	
		@tparam T The type of component to fetch.
		@param entity The entity to fetch the component from.
	*/
	template <class T>
	T* GetComponent(EntityID entity);

	// Triggers commit events in systems
	// TODO: Replace completely by EComponentCreated?
	int CommitEntity(EntityID entity);

	/** Get all components of a specific type.
	
		@tparam T The type of component to query.
		@returns A list of components of type T in the World.
	*/
	template <class T>
	std::list<std::shared_ptr<Component>>* GetComponentsOfType();

	/*std::vector<EntityID> GetEntityChildren(EntityID entity);*/

	/** Moves the world simulation forward one tick. 
		
		@param dt Time since last frame in seconds.
	*/
	virtual void Update(double dt);
	// Recursively update through the scene graph
	// TODO: Private?
	void RecursiveUpdate(std::shared_ptr<System> system, double dt, EntityID parentEntity);

	/** Fetch the scene graph.
	
		@returns An std::unordered_map of entities to their parents.
	*/
	std::unordered_map<EntityID, EntityID>* GetEntities() { return &m_EntityParents; }

	dd::SystemFactory SystemFactory;
	dd::ComponentFactory ComponentFactory;

protected:
	std::shared_ptr<dd::EventBroker> EventBroker;
	std::shared_ptr<dd::ResourceManager> ResourceManager;

	std::unordered_map<std::string, std::shared_ptr<System>> m_Systems;

	EntityID m_LastEntityID;
	std::stack<EntityID> m_RecycledEntityIDs;

	std::unordered_map<EntityID, EntityID> m_EntityParents; // child -> parent
	std::unordered_map<EntityID, std::list<EntityID>> m_EntityChildren; // parent -> child
	std::unordered_map<EntityID, std::unordered_map<std::string, boost::any>> m_EntityProperties;
	std::unordered_map<std::string, std::list<std::shared_ptr<Component>>> m_ComponentsOfType;
	std::unordered_map<EntityID, std::map<std::string, std::shared_ptr<Component>>> m_EntityComponents;

	// Internal: Add a component to an entity
	void AddComponent(EntityID entity, std::string componentType, std::shared_ptr<Component> component);

	std::set<EntityID> m_EntitiesToRemove;
	void ProcessEntityRemovals();

	EntityID GenerateEntityID();

	void RecycleEntityID(EntityID id);

private:
	//Benchmarking
	struct TimeMeasure	{
		double EventTime;
		double SystemTime;
		double RSystemTime;
	};
	std::map<std::string, TimeMeasure> m_typeToTimeMap;
};

template <class T>
std::list<std::shared_ptr<Component>>* World::GetComponentsOfType()
{
	const char* componentType = typeid(T).name();

	auto it = m_ComponentsOfType.find(componentType);
	if (it == m_ComponentsOfType.end())
		return nullptr;

	return &it->second;
}

template <class T>
std::shared_ptr<T> World::GetSystem()
{
	const char* systemType = typeid(T).name();

	if (m_Systems.find(systemType) == m_Systems.end())
	{
		LOG_WARNING("Tried to get pointer to unregistered system \"%s\"!", systemType);
		return nullptr;
	}

	return std::static_pointer_cast<T>(m_Systems.at(systemType));
}

template <class T>
std::shared_ptr<T> World::AddComponent(EntityID entity)
{
	const char* componentType = typeid(T).name();

	std::shared_ptr<T> component = std::shared_ptr<T>(static_cast<T*>(ComponentFactory.Create<T>()));
	if (component == nullptr)
	{
		LOG_ERROR("Failed to attach invalid component \"%s\" to entity #%i", componentType, entity);
		return nullptr;
	}

	AddComponent(entity, componentType, component);
	
	return component;
}

template <class T>
void World::RemoveComponent(EntityID entity)
{
	const char* componentType = typeid(T).name();

	auto it = m_EntityComponents[entity].find(componentType);
	if (it == m_EntityComponents[entity].end())
		return;

	auto component = it->second;

	component->Entity = 0;
	m_ComponentsOfType[componentType].remove(component);
	m_EntityComponents[entity].erase(it);
	for (auto pair : m_Systems)
	{
		auto system = pair.second;
		system->OnComponentRemoved(entity, componentType, component.get());
	}
}

template <class T>
T* World::GetComponent(EntityID entity)
{
	auto components = m_EntityComponents[entity]; 

	auto it = components.find(typeid(T).name());
	if (it != components.end())
	{
		return static_cast<T*>(it->second.get());
	}
	else
	{
		return nullptr;
	}
}

}

#endif // World_h__
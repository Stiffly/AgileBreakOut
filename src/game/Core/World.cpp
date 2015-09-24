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
#include "Core/World.h"

//TODO Delete this please

void dd::World::RecycleEntityID(EntityID id)
{
	m_RecycledEntityIDs.push(id);
}

EntityID dd::World::GenerateEntityID()
{
	if (!m_RecycledEntityIDs.empty())
	{
		EntityID id = m_RecycledEntityIDs.top();
		m_RecycledEntityIDs.pop();
		return id;
	}
	else
	{
		return ++m_LastEntityID;
	}
}

void dd::World::RecursiveUpdate(std::shared_ptr<System> system, double dt, EntityID parentEntity)
{
	for (auto &pair : m_EntityParents)
	{
		EntityID child = pair.first;
		EntityID parent = pair.second;

		system->UpdateEntity(dt, child, parent);
			//RecursiveUpdate(system, dt, child);
	}
}

void dd::World::Update(double dt)
{
	for (auto pair : m_Systems)
	{
		const std::string &type = pair.first;
		auto system = pair.second;

		std::map<std::string, TimeMeasure>::iterator it = m_typeToTimeMap.find(type.c_str());
		if (it == m_typeToTimeMap.end()) {
			//Does not contain item
			TimeMeasure t;
			t.EventTime = 0;
			t.SystemTime = 0;
			t.RSystemTime = 0;
			m_typeToTimeMap[type.c_str()] = t;
		}

		double start = glfwGetTime();
		EventBroker->Process(type);
		double stop = glfwGetTime();
		double t1 = stop - start;
		m_typeToTimeMap[type.c_str()].EventTime += t1;

		start = glfwGetTime();
		system->Update(dt);
		stop = glfwGetTime();
		double t2 = stop - start;
		m_typeToTimeMap[type.c_str()].SystemTime += t2;

		start = glfwGetTime();
		RecursiveUpdate(system, dt, 0);
		stop = glfwGetTime();
		double t3 = stop - start;
		m_typeToTimeMap[type.c_str()].RSystemTime += t3;
	}
	EventBroker->Process<World>();
	ProcessEntityRemovals();
}

//std::vector<EntityID> GetEntityChildren(EntityID entity);
//{
//	std::vector<EntityID> children;
//	auto range = m_SceneGraph.equal_range(entity);
//	for (auto it = range.first; it != range.second; ++it)
//		children.push_back(it->second);
//	return children;
//}

EntityID dd::World::GetEntityParent(EntityID entity)
{
	auto it = m_EntityParents.find(entity);
	return it == m_EntityParents.end() ? 0 : it->second;
}

EntityID dd::World::GetEntityBaseParent(EntityID entity)
{
	EntityID parent = GetEntityParent(entity);
	if (parent == 0)
		return entity;
	else
		return GetEntityBaseParent(parent);
}

bool dd::World::ValidEntity(EntityID entity)
{
	return m_EntityParents.find(entity) != m_EntityParents.end()
		&& m_EntitiesToRemove.find(entity) == m_EntitiesToRemove.end();
}

void dd::World::RemoveEntity(EntityID entity)
{
	m_EntitiesToRemove.insert(entity);

	auto it = m_EntityChildren.find(entity);
	if (it != m_EntityChildren.end())
	{
		for (auto entity : it->second)
		{
			RemoveEntity(entity);
		}
	}
}

void dd::World::ProcessEntityRemovals()
{
	for (auto entity : m_EntitiesToRemove)
	{
		m_EntityParents.erase(entity);
		m_EntityChildren.erase(entity);
		// Remove components
		for (auto pair : m_EntityComponents[entity])
		{
			auto type = pair.first;
			auto component = pair.second;
			// Trigger events
			for (auto pair : m_Systems)
			{
				auto system = pair.second;
				system->OnComponentRemoved(entity, type, component.get());
			}
			m_ComponentsOfType[type].remove(component);
		}
		m_EntityComponents.erase(entity);
		RecycleEntityID(entity);

		// Trigger events
		for (auto pair : m_Systems)
		{
			auto system = pair.second;
			system->OnEntityRemoved(entity);
		}
	}
	m_EntitiesToRemove.clear();
}

EntityID dd::World::CreateEntity(EntityID parent /*= 0*/)
{
	EntityID newEntity = GenerateEntityID();
	m_EntityParents[newEntity] = parent;
	m_EntityChildren[parent].push_back(newEntity);
	return newEntity;
}

void dd::World::Initialize()
{
	RegisterSystems();
	AddSystems();
	for (auto pair : m_Systems)
	{
		auto system = pair.second;
		system->RegisterComponents(&ComponentFactory);
		system->RegisterResourceTypes(ResourceManager);
		system->Initialize();
	}

	EVENT_SUBSCRIBE_MEMBER(m_EKeyDown, &World::OnKeyDown);
}

int dd::World::CommitEntity(EntityID entity)
{
	for (auto pair : m_Systems)
	{
		auto system = pair.second;
		system->OnEntityCommit(entity);
	}
}

void dd::World::AddComponent(EntityID entity, std::string componentType, std::shared_ptr<Component> component)
{
	component->Entity = entity;
	m_ComponentsOfType[componentType].push_back(component);
	m_EntityComponents[entity][componentType] = component;

	Events::ComponentCreated e;
	e.Entity = entity;
	e.Component = component;
	EventBroker->Publish(e);
}

EntityID dd::World::CloneEntity(EntityID entity, EntityID parent /* = 0 */)
{
	int clone = CreateEntity(parent);

	for (auto pair : m_EntityComponents[entity])
	{
		auto type = pair.first;
		auto component = std::shared_ptr<Component>(ComponentFactory.Copy(type, pair.second.get()));
		if (component != nullptr)
		{
			AddComponent(clone, type, component);
		}
	}

	auto itChildren = m_EntityChildren.find(entity);
	if (itChildren != m_EntityChildren.end())
	{
		for (EntityID child : itChildren->second)
		{
			CloneEntity(child, clone);
		}
	}

	CommitEntity(clone);

	return clone;
}

std::list<EntityID> dd::World::GetEntityChildren(EntityID entity)
{
	auto it = m_EntityChildren.find(entity);
	if (it == m_EntityChildren.end())
	{
		return std::list<EntityID>();
	}
	else
	{
		return it->second;
	}
}

void dd::World::SetEntityParent(EntityID entity, EntityID newParent)
{
	EntityID currentParent = m_EntityParents[entity];
	m_EntityChildren[currentParent].remove(entity);
	m_EntityParents[entity] = newParent;
	m_EntityChildren[newParent].push_back(entity);
}

bool dd::World::OnKeyDown(const dd::Events::KeyDown &event)
{
	if (event.KeyCode == 78)
	{
		double eventTot = 0;
		double updateTot = 0;
		double rUpdateTot = 0;
		double total = 0;

		for (auto item : m_typeToTimeMap)
		{
			eventTot += item.second.EventTime;
			updateTot += item.second.SystemTime;
			rUpdateTot += item.second.RSystemTime;
		}
		total = eventTot + updateTot + rUpdateTot;

		std::ofstream outFile;
		outFile.open("../tools/system-metrics.txt");
		outFile.clear();
		outFile << "----------------------- Time measurements -----------------------\n";
		for (auto item : m_typeToTimeMap)
		{
			outFile << "Type: " << item.first.c_str() << std::endl;
			outFile << "Events: " << item.second.EventTime << " s. " << item.second.EventTime / eventTot  * 100 << " % of total event time.\n";
			outFile << "Update: " << item.second.SystemTime << " s. " << item.second.SystemTime / updateTot  * 100 << " % of total update time.\n";
			outFile << "Recursive Update: " << item.second.RSystemTime << " s. " << item.second.RSystemTime / rUpdateTot  * 100 << " % of total recursive update time.\n";
			double totalComplexity = (item.second.EventTime + item.second.SystemTime + item.second.RSystemTime) / total * 100;
			outFile << "Total: " << totalComplexity << "% total system time." << std::endl << std::endl;
		}
		outFile  << "-----------------------------------------------------------------";
		outFile.close();
		return true;

	}
	//return false;
}

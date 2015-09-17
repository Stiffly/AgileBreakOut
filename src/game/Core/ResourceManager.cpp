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
#include "Core/ResourceManager.h"

std::unordered_map<std::string, std::function<dd::Resource*(std::string)>> dd::ResourceManager::m_FactoryFunctions;
std::unordered_map<std::pair<std::string, std::string>, dd::Resource*> dd::ResourceManager::m_ResourceCache;
std::unordered_map<std::string, dd::Resource*> dd::ResourceManager::m_ResourceFromName;
std::unordered_map<dd::Resource*, dd::Resource*> dd::ResourceManager::m_ResourceParents;
unsigned int dd::ResourceManager::m_CurrentResourceTypeID = 0;
std::unordered_map<std::string, unsigned int> dd::ResourceManager::m_ResourceTypeIDs;
std::unordered_map<unsigned int, unsigned int> dd::ResourceManager::m_ResourceCount;
bool dd::ResourceManager::m_Preloading = false;
dd::FileWatcher dd::ResourceManager::m_FileWatcher;

unsigned int dd::ResourceManager::GetTypeID(std::string resourceType)
{
	if (m_ResourceTypeIDs.find(resourceType) == m_ResourceTypeIDs.end())
	{
		m_ResourceTypeIDs[resourceType] = m_CurrentResourceTypeID++;
	}
	return m_ResourceTypeIDs[resourceType];
}


void dd::ResourceManager::Reload(std::string resourceName)
{
	auto it = m_ResourceFromName.find(resourceName);
	if (it != m_ResourceFromName.end()) {
		LOG_INFO("Reloading resource \"%s\"", resourceName.c_str());
		Resource* resource = it->second;
		resource->Reload();

		// Notify parent
		auto it2 = m_ResourceParents.find(resource);
		if (it2 != m_ResourceParents.end())
		{
			it2->second->OnChildReloaded(resource);
		}
	}
}

unsigned int dd::ResourceManager::GetNewResourceID(unsigned int typeID)
{
	return m_ResourceCount[typeID]++;
}

bool dd::ResourceManager::IsResourceLoaded(std::string resourceType, std::string resourceName)
{
	return m_ResourceCache.find(std::make_pair(resourceType, resourceName)) != m_ResourceCache.end();
}

void dd::ResourceManager::fileWatcherCallback(std::string path, FileWatcher::FileEventFlags flags)
{
	if (flags & FileWatcher::FileEventFlags::SizeChanged
		|| flags & FileWatcher::FileEventFlags::TimestampChanged)
	{
		auto it = m_ResourceFromName.find(path);
		if (it != m_ResourceFromName.end())
		{
			Reload(path);
		}
	}
}

void dd::ResourceManager::Update()
{
	m_FileWatcher.Check();
}

//Niklas is trying to get frames to work.
dd::Resource* dd::ResourceManager::CreateResource(std::string resourceType, std::string resourceName)
{
	auto facIt = m_FactoryFunctions.find(resourceType);
	if (facIt == m_FactoryFunctions.end())
	{
		LOG_ERROR("Failed to load resource \"%s\" of type \"%s\": Type not registered", resourceName.c_str(), resourceType.c_str());
		return nullptr;
	}

	// Call the factory function
	Resource* resource = facIt->second(resourceName);
	// Store IDs
	resource->TypeID = GetTypeID(resourceType);
	resource->ResourceID = GetNewResourceID(resource->TypeID);
	// Cache
	m_ResourceCache[std::make_pair(resourceType, resourceName)] = resource;

	return resource;
}
//Niklas is trying to get frames to work./end

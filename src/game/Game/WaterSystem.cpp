//
// Created by Adniklastrator on 2015-10-20.
//

#include "PrecompiledHeader.h"
#include "Game/WaterSystem.h"

void dd::Systems::WaterSystem::Initialize()
{
	EVENT_SUBSCRIBE_MEMBER(m_EPause, &WaterSystem::OnPause);
	EVENT_SUBSCRIBE_MEMBER(m_EResume, &WaterSystem::OnResume);
	EVENT_SUBSCRIBE_MEMBER(m_ERelevantObjectCreated, &WaterSystem::OnRelevantObjectCreated);
    EVENT_SUBSCRIBE_MEMBER(m_EStageCleared, &WaterSystem::OnStageCleared);
	EVENT_SUBSCRIBE_MEMBER(m_ERaiseWater, &WaterSystem::OnRaiseWater);
	EVENT_SUBSCRIBE_MEMBER(m_ERaiseWaterWall, &WaterSystem::OnRaiseWaterWall);

	//Water test
	{
		auto t_waterBody = m_World->CreateEntity();
		auto transform = m_World->AddComponent<Components::Transform>(t_waterBody);
		transform->Position = glm::vec3(0.f, -3.5f, -10.f);
		transform->Scale = glm::vec3(7.0f, 1.5f, 1.f);
		auto water = m_World->AddComponent<Components::WaterVolume>(t_waterBody);
		auto body = m_World->AddComponent<Components::RectangleShape>(t_waterBody);
		m_World->CommitEntity(t_waterBody);
	}
}

void dd::Systems::WaterSystem::Update(double dt)
{
	if (m_Pause) {
		return;
	}
	
}

void dd::Systems::WaterSystem::UpdateEntity(double dt, EntityID entity, EntityID parent)
{
	if (m_Pause) {
		return;
	}

	auto templateCheck = m_World->GetComponent<Components::Template>(entity);
	if (templateCheck != nullptr){ return; }

	if (m_BottomWall == 0) {
		for (auto it = m_World->GetEntities()->begin(); it != m_World->GetEntities()->end(); it++) {
			if (m_World->GetProperty<std::string>(it->first, "Name") == "BottomWall") {
				m_BottomWall = entity;
				break;
			}
		}
	}
}

bool dd::Systems::WaterSystem::OnPause(const dd::Events::Pause &event)
{
	/*if (event.Type != "WaterSystem" && event.Type != "All") {
		return false;
	}*/
	m_Pause = true;
    return true;
}
bool dd::Systems::WaterSystem::OnResume(const dd::Events::Resume &event)
{
	/*if (event.Type != "WaterSystem" && event.Type != "All") {
	return false;
	}*/
	m_Pause = false;
	return true;
}

bool dd::Systems::WaterSystem::OnRelevantObjectCreated(const dd::Events::RelevantObjectCreated &event)
{
	if (event.ObjectName == "BottomWall") {
		m_BottomWall = event.ObjectID;
	}
	return true;
}

bool dd::Systems::WaterSystem::OnStageCleared(const dd::Events::StageCleared &event)
{
	
	return true;
}

bool dd::Systems::WaterSystem::OnArrivedAtNewStage(const dd::Events::ArrivedAtNewStage &event)
{
	
	return true;
}

bool dd::Systems::WaterSystem::OnRaiseWater(const dd::Events::RaiseWater &event)
{
	{
		auto t_waterBody = m_World->CreateEntity();
		auto transform = m_World->AddComponent<Components::Transform>(t_waterBody);
		transform->Position = glm::vec3(0.f, -3.5f, -10.f);
		transform->Scale = glm::vec3(7.0f, event.Amount * 1.6f, 1.f);
		auto water = m_World->AddComponent<Components::WaterVolume>(t_waterBody);
		auto body = m_World->AddComponent<Components::RectangleShape>(t_waterBody);
		m_World->CommitEntity(t_waterBody);
	}

	return true;
}

bool dd::Systems::WaterSystem::OnRaiseWaterWall(const dd::Events::RaiseWaterWall &event) 
{
	auto transform = m_World->GetComponent<Components::Transform>(m_BottomWall);
	if (transform != nullptr) {
		glm::vec3 raise = glm::vec3(0, event.Amount, 0);
		Events::Move e;
		e.Entity = m_BottomWall;
		e.GoalPosition = transform->Position + raise;
		e.Queue = true;
		e.Speed = 0.2;
		EventBroker->Publish(e);
	} else {
		LOG_ERROR("There is no BottomWall in WaterSystem");
	}

	return true;
}
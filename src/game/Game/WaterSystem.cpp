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

	m_RaiseWater = ResourceManager::Load<ConfigFile>("Config.ini")->GetValue<bool>("Debug.RaiseWater", false);

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

	m_BrickPartCounters =
	{0, 0, 0, 0, 0,
	 0, 0, 0, 0, 0 };
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

	auto brickPart = m_World->GetComponent<Components::BrickPart>(entity);
	if (brickPart != nullptr) {
		auto transform = m_World->GetComponent<Components::Transform>(entity);
		if (m_RaiseWater) {
			if (!brickPart->Submerged) {
				if (transform->Position.y < m_SubmergedHeight) {
					brickPart->Submerged = true;
					m_BrickPartCounters[brickPart->NumberOfPartsInBrick]++;
					std::cout << "Parts: " << m_BrickPartCounters[brickPart->NumberOfPartsInBrick] << " of " << brickPart->NumberOfPartsInBrick << std::endl;
					if (m_BrickPartCounters[brickPart->NumberOfPartsInBrick] >= brickPart->NumberOfPartsInBrick) {
						m_BrickPartCounters[brickPart->NumberOfPartsInBrick] = 0;
						Events::RaiseWater e;
						e.Amount = 0.2;
						e.Speed = 0.2;
						EventBroker->Publish(e);
					}
				}
			}
		}
		if (transform->Position.y < -10) {
			m_World->RemoveEntity(entity);
			return;
		}
	}

	if (m_BottomWall == 0) {
		for (auto it = m_World->GetEntities()->begin(); it != m_World->GetEntities()->end(); it++) {
			if (m_World->GetProperty<std::string>(it->first, "Name") == "BottomWall") {
				m_BottomWall = entity;
				break;
			}
		}
	}

	auto newWater = m_World->GetComponent<Components::NewWaterVolume>(entity);
	if (newWater != nullptr) {
		if (m_RemoveWater) {
			//m_World->RemoveComponent<Components::WaterVolume>(entity);
			//m_World->RemoveComponent<Components::RectangleShape>(entity);
			//m_World->RemoveComponent<Components::WaterVolume>(entity);
			//m_World->RemoveEntity(entity);
			//m_World->RemoveComponent<Components::>();
		}
		/*auto transform = m_World->AddComponent<Components::Transform>(entity);
		if (transform->Position.y < -10) {
			m_World->RemoveEntity(entity);
		}*/
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
	m_SubmergedHeight = m_SubmergedOriginalHeight;
	m_RemoveWater = true;
	return true;
}

bool dd::Systems::WaterSystem::OnArrivedAtNewStage(const dd::Events::ArrivedAtNewStage &event)
{
	m_RemoveWater = false;
	return true;
}

bool dd::Systems::WaterSystem::OnRaiseWater(const dd::Events::RaiseWater &event)
{
	{
		auto t_waterBody = m_World->CreateEntity();
		auto transform = m_World->AddComponent<Components::Transform>(t_waterBody);
		transform->Position = glm::vec3(0.f, m_SubmergedHeight, -10.f);
		transform->Scale = glm::vec3(7.0f, event.Amount * 1.4f, 1.f);
		auto water = m_World->AddComponent<Components::WaterVolume>(t_waterBody);
		auto newWater = m_World->AddComponent<Components::NewWaterVolume>(t_waterBody); //Meaning it isn't the original water we had.
		auto body = m_World->AddComponent<Components::RectangleShape>(t_waterBody);
		m_World->CommitEntity(t_waterBody);
	}

	m_SubmergedHeight += event.Amount;

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
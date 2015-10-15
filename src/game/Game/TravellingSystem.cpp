//
// Created by Adniklastrator on 2015-10-13.
//

#include "PrecompiledHeader.h"
#include "Game/TravellingSystem.h"

void dd::Systems::TravellingSystem::Initialize()
{
	EVENT_SUBSCRIBE_MEMBER(m_EPause, &TravellingSystem::OnPause);
    EVENT_SUBSCRIBE_MEMBER(m_EStageCleared, &TravellingSystem::OnStageCleared);
}

void dd::Systems::TravellingSystem::Update(double dt)
{
	if (m_Pause) {
		return;
	}
	if (m_Travelling) {
		if (m_DistanceTravelled > 12.f) {
			m_DistanceTravelled = 0;
			m_Travelling = false;
			Events::ArrivedAtNewStage e;
			EventBroker->Publish(e);
		}

		m_DistanceTravelled += 6.0f * dt;
	}
}

void dd::Systems::TravellingSystem::UpdateEntity(double dt, EntityID entity, EntityID parent)
{
	if (m_Travelling) {
		auto travels = m_World->GetComponent<Components::Travels>(entity);
		if (travels != nullptr) {
			auto transform = m_World->GetComponent<Components::Transform>(entity);
			if (transform != nullptr) {
				transform->Position.y -= 6.0f * dt;
			}
		}
	}
}

bool dd::Systems::TravellingSystem::OnPause(const dd::Events::Pause &event)
{
	if (event.Type != "TravellingSystem" && event.Type != "All") {
		return false;
	}

    if (IsPaused()) {
        SetPause(false);
    } else {
        SetPause(true);
    }
    return true;
}

bool dd::Systems::TravellingSystem::OnStageCleared(const dd::Events::StageCleared &event)
{
	if (event.ClearedStage < 5) {
		m_Travelling = true;
	}
	return true;
}
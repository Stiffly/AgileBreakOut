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
		if (m_DistanceTravelled > 12.f) { //NextLevelDistance
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
	auto templateCheck = m_World->GetComponent<Components::Template>(entity);
	if (templateCheck != nullptr){ return; }

	if (m_Travelling) {
		auto travels = m_World->GetComponent<Components::Travels>(entity);
		if (travels != nullptr) {
			if (!travels->CurrentlyTraveling) {
				auto transform = m_World->GetComponent<Components::Transform>(entity);
				if (transform != nullptr) {
					Events::Move e;
					e.Entity = entity;
					glm::vec3 position = transform->Position;
					glm::vec3 goalPosition = position;
					e.Queue = false;
					e.Speed = 6;
					goalPosition.y -= 12; //NextLevelDistance
					if (goalPosition.y < -34.6) { //Halfpipe Value
						auto background = m_World->GetComponent<Components::Background>(entity);
						if (background != nullptr) {
							float yValue = position.y; // A negative value between -34.6 and -22.6;
							yValue += 22.6; // A positive value between -12 and 0. The distance it should travel from 34.6;
							background->distanceLeftToCorrectTravelPosition = yValue;
						} else {
							m_World->RemoveEntity(entity);
							return;
						}
					}
					e.GoalPosition = goalPosition;
					
					travels->CurrentlyTraveling = true;
					EventBroker->Publish(e);
					//transform->Position.y -= 6.0f * dt;
				}
			}
		}
	} else {
		auto travels = m_World->GetComponent<Components::Travels>(entity);
		if (travels != nullptr) {
			travels->CurrentlyTraveling = false;
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

bool dd::Systems::TravellingSystem::OnArrivedAtNewStage(const dd::Events::ArrivedAtNewStage &event)
{
	
	return true;
}
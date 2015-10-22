//
// Created by Adniklastrator on 2015-10-14.
//

#include "PrecompiledHeader.h"
#include "Game/ScreenShakeSystem.h"


void dd::Systems::ScreenShakeSystem::Initialize()
{
	std::random_device rd;
	m_RandomGenerator = std::mt19937(rd());
    EVENT_SUBSCRIBE_MEMBER(m_EPause, &ScreenShakeSystem::OnPause);
	EVENT_SUBSCRIBE_MEMBER(m_EResume, &ScreenShakeSystem::OnResume);
    EVENT_SUBSCRIBE_MEMBER(m_EScreenShake, &ScreenShakeSystem::OnScreenShake);

	InitializeObjects();
}

void dd::Systems::ScreenShakeSystem::InitializeObjects()
{
	m_Pause = false;
	m_ScreenShake = false;
	m_ShakeIntensity = 0;
	m_CameraPosition = glm::vec3(0, 0, 0);
	m_ShakeRepresentativeIntensity = 0;
	m_ShakeTimer = 0;
	m_ShakeEndTime = 0;
	m_TimeTakenToCoolDown = 0;
	m_CoolerMultiplier = 0;
	m_Random = glm::vec3(0, 0, 0);
}

void dd::Systems::ScreenShakeSystem::UpdateEntity(double dt, EntityID entity, EntityID parent)
{
    if (IsPaused()) {
        return;
    }

	/*auto camera = m_World->GetComponent<Components::Camera>(entity);
	
	if (camera != nullptr) {
		auto transform = m_World->GetComponent<Components::Transform>(entity);

		if (m_ScreenShake) {
			m_CameraPosition = transform->Position - m_Random;
			
			m_ShakeTimer += dt;
			if (m_ShakeTimer >= m_ShakeEndTime) {
				//std::cout << "Used: " << m_ShakeIntensity << " True: " << m_ShakeRepresentativeIntensity << std::endl;
				m_ShakeRepresentativeIntensity -= m_ShakeRepresentativeIntensity * m_CoolerMultiplier * dt;
				m_ShakeIntensity = m_ShakeRepresentativeIntensity;
				if (m_ShakeIntensity <= 0) {
					m_ScreenShake = false;
					transform->Position = glm::vec3(0, 0, 0);
					return;
				}
			}
			else {
				//std::cout << "Timer: " << m_ShakeTimer << " End: " << m_ShakeEndTime << std::endl;
			}
			std::uniform_real_distribution<float> dist(-m_ShakeIntensity, m_ShakeIntensity);
			//float impulseStrength = dist(m_RandomGenerator);
			m_Random.x = dist(m_RandomGenerator) / 20;
			m_Random.y = dist(m_RandomGenerator) / 20;

			//float half = m_ShakeIntensity / 2;

			//m_Random.x = (m_Random.x - half) / 20;
			//m_Random.y = (m_Random.y - half) / 20;

			//std::cout << "X: " << randomX << " Y: " << randomY << std::endl;
			transform->Position = glm::vec3(m_Random);
		} else {
			m_CameraPosition = transform->Position;
		}
	}*/
}

bool dd::Systems::ScreenShakeSystem::OnPause(const dd::Events::Pause &event)
{
	/*if (event.Type != "ScreenShakeSystem" && event.Type != "All") {
		return false;
	}*/

    m_Pause = true;
    return true;
}
bool dd::Systems::ScreenShakeSystem::OnResume(const dd::Events::Resume &event)
{
	/*if (event.Type != "ScreenShakeSystem" && event.Type != "All") {
	return false;
	}*/
	m_Pause = false;
	return true;
}

bool dd::Systems::ScreenShakeSystem::OnScreenShake(const dd::Events::ScreenShake &event)
{
	m_ScreenShake = true;
	m_ShakeIntensity = event.Intensity;
	m_ShakeRepresentativeIntensity = event.Intensity;
	m_ShakeEndTime = event.Time;
	m_ShakeTimer = 0;
	m_TimeTakenToCoolDown = event.TimeTakenToCoolDown;
	m_CoolerMultiplier = m_ShakeIntensity / m_TimeTakenToCoolDown;
	return true;
}
//
// Created by Adniklastrator on 2015-10-14.
//

#ifndef DAYDREAM_SCREENSHAKESYSTEM_H
#define DAYDREAM_SCREENSHAKESYSTEM_H


#include "Core/System.h"
#include "Core/CTransform.h"
#include "Core/CTemplate.h"
#include "Core/EventBroker.h"
#include "Core/World.h"
#include "Game/EPause.h"
#include "Game/EScreenShake.h"
//#include "Core/Camera.h" Camera Component


namespace dd
{

namespace Systems
{

class ScreenShakeSystem : public System
{
public:
    ScreenShakeSystem(World* world, std::shared_ptr<dd::EventBroker> eventBroker)
    : System(world, eventBroker)
    { }

    void Initialize() override;
	void InitializeObjects();

	void UpdateEntity(double dt, EntityID entity, EntityID parent) override;
    void FlipPause();

    bool IsPaused() const { return m_Pause; }
    void SetPause(const bool& pause) { m_Pause = pause; }
private:
    bool m_Pause;
	bool m_ScreenShake;
	int m_ShakeIntensity;
	glm::vec3 m_CameraPosition;
	float m_ShakeRepresentativeIntensity;
	float m_ShakeTimer;
	float m_ShakeEndTime;
	float m_TimeTakenToCoolDown;
	float m_CoolerMultiplier;	
	glm::vec3 m_Random;

    dd::EventRelay<ScreenShakeSystem, dd::Events::Pause> m_EPause;
	dd::EventRelay<ScreenShakeSystem, dd::Events::ScreenShake> m_EScreenShake;
    bool OnPause(const dd::Events::Pause &event);
    bool OnScreenShake(const dd::Events::ScreenShake &event);



};

}

}

#endif //DAYDREAM_SCREENSHAKESYSTEM_H

//
// Created by Adniklastrator on 2015-10-15.
//

#ifndef DAYDREAM_KRAKENSYSTEM_H
#define DAYDREAM_KRAKENSYSTEM_H

#include <random>

#include "Core/System.h"
#include "Core/CTransform.h"
#include "Core/CTemplate.h"
#include "Core/EventBroker.h"
#include "Core/World.h"
#include "Rendering/CModel.h"
#include "Physics/CRectangleShape.h"
#include "Physics/CCircleShape.h"
#include "Physics/CPhysics.h"
#include "Physics/EContact.h"
#include "Game/EPause.h"
#include "Game/EResume.h"
#include "Game/EKrakenAppear.h"
#include "Game/EKrakenAttack.h"
#include "Game/EBrickGenerating.h"
#include "Game/CTravels.h"
#include "Game/CKraken.h"
#include "Sound/CCollisionSound.h"
//#include "Core/Camera.h" Camera Component


namespace dd
{

namespace Systems
{

class KrakenSystem : public System
{
public:
    KrakenSystem(World* world, std::shared_ptr<dd::EventBroker> eventBroker)
    : System(world, eventBroker)
    { }

    void Initialize() override;
	void InitializeObjects();

	void Update(double dt) override;
	void UpdateEntity(double dt, EntityID entity, EntityID parent) override;

    bool IsPaused() const { return m_Pause; }
    void SetPause(const bool& pause) { m_Pause = pause; }
private:
	std::mt19937 m_RandomGenerator;

	bool m_Pause = false;
	bool m_KrakenBattle = false;
	bool m_ReturnToIdle = false;
	EntityID m_KrakenTemplate;
	std::string m_Action = "Idle";
	float m_KrakenTimer = 0;
	float m_KrakenSecondsToAction = 1;
	const int Idle = 1;

	std::array<int, 14> m_Bricks;
	std::array<int, 14> m_Colors;

    dd::EventRelay<KrakenSystem, dd::Events::Pause> m_EPause;
	dd::EventRelay<KrakenSystem, dd::Events::Resume> m_EResume;
	dd::EventRelay<KrakenSystem, dd::Events::Contact> m_EContact;
	dd::EventRelay<KrakenSystem, dd::Events::KrakenAppear> m_EKrakenAppear;
	dd::EventRelay<KrakenSystem, dd::Events::KrakenAttack> m_EKrakenAttack;
	dd::EventRelay<KrakenSystem, dd::Events::BrickGenerating> m_EBrickGenerating;
    bool OnPause(const dd::Events::Pause &event);
	bool OnResume(const dd::Events::Resume &event);
	bool OnContact(const dd::Events::Contact &event);
	bool OnKrakenAppear(const dd::Events::KrakenAppear &event);
	bool OnKrakenAttack(const dd::Events::KrakenAttack &event);
	bool OnBrickGenerating(const dd::Events::BrickGenerating &event);
	void GetBrickSet();
};

}

}

#endif //DAYDREAM_KRAKENSYSTEM_H

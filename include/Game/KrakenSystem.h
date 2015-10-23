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
#include "Physics/ESetImpulse.h"
#include "Physics/ECreateParticleSequence.h"
#include "Game/EPause.h"
#include "Game/EResume.h"
#include "Game/EComboEvent.h"
#include "Game/EScoreEvent.h"
#include "Game/EKrakenAppear.h"
#include "Game/EKrakenAttack.h"
#include "Game/EKrakenHit.h"
#include "Game/EKrakenDefeated.h"
#include "Game/EBrickGenerating.h"
#include "Game/EArrivedAtNewStage.h"
#include "Game/CTravels.h"
#include "Game/CKraken.h"
#include "Game/CBall.h"
#include "Game/CProjectile.h"
#include "Sound/CCollisionSound.h"
#include "Rendering/CAnimation.h"
#include "Rendering/CSprite.h"
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
	bool m_KrakenHasArrived = false;
	bool m_ReturnToIdle = false;
	bool m_RandomKraken = false;
	EntityID m_KrakenTemplate;
	std::string m_Action = "Idle";
	int m_NumberOfActions;
	float m_KrakenTimer = 0;
	float m_KrakenMaxSecondsToAction = 10;
	float m_KrakenMinSecondsToAction = 10;
	float m_KrakenCurrentSecondsToAction = 15;
	const int Idle = 1;

	int m_Set = 0;
	int m_SetCluster = 0;

	std::array<int, 42> m_Bricks;
	std::array<int, 42> m_Colors;

    dd::EventRelay<KrakenSystem, dd::Events::Pause> m_EPause;
	dd::EventRelay<KrakenSystem, dd::Events::Resume> m_EResume;
	dd::EventRelay<KrakenSystem, dd::Events::Contact> m_EContact;
	dd::EventRelay<KrakenSystem, dd::Events::KrakenAppear> m_EKrakenAppear;
	dd::EventRelay<KrakenSystem, dd::Events::KrakenAttack> m_EKrakenAttack;
	dd::EventRelay<KrakenSystem, dd::Events::KrakenHit> m_EKrakenHit;
	dd::EventRelay<KrakenSystem, dd::Events::KrakenDefeated> m_EKrakenDefeated;
	dd::EventRelay<KrakenSystem, dd::Events::BrickGenerating> m_EBrickGenerating;
	dd::EventRelay<KrakenSystem, dd::Events::ArrivedAtNewStage> m_EArrivedAtNewStage;
    bool OnPause(const dd::Events::Pause &event);
	bool OnResume(const dd::Events::Resume &event);
	bool OnContact(const dd::Events::Contact &event);
	bool OnKrakenAppear(const dd::Events::KrakenAppear &event);
	bool OnKrakenAttack(const dd::Events::KrakenAttack &event);
	bool OnKrakenHit(const dd::Events::KrakenHit &event);
	bool OnKrakenDefeated(const dd::Events::KrakenDefeated &event);
	bool OnBrickGenerating(const dd::Events::BrickGenerating &event);
	bool OnArrivedAtNewStage(const dd::Events::ArrivedAtNewStage &event);
	void GetBrickSet();
};

}

}

#endif //DAYDREAM_KRAKENSYSTEM_H

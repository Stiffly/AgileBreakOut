//
// Created by Adniklastrator on 2015-10-15.
//

#ifndef DAYDREAM_KRAKENSYSTEM_H
#define DAYDREAM_KRAKENSYSTEM_H


#include "Core/System.h"
#include "Core/CTransform.h"
#include "Core/CTemplate.h"
#include "Core/EventBroker.h"
#include "Core/World.h"
#include "Rendering/CModel.h"
#include "Physics/CRectangleShape.h"
#include "Physics/CCircleShape.h"
#include "Physics/CPhysics.h"
#include "Game/EPause.h"
#include "Game/EKrakenAppear.h"
#include "Game/EKrakenAttack.h"
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

	void UpdateEntity(double dt, EntityID entity, EntityID parent) override;

    bool IsPaused() const { return m_Pause; }
    void SetPause(const bool& pause) { m_Pause = pause; }
private:
	bool m_Pause = false;
	EntityID m_KrakenTemplate;

    dd::EventRelay<KrakenSystem, dd::Events::Pause> m_EPause;
	dd::EventRelay<KrakenSystem, dd::Events::KrakenAppear> m_EKrakenAppear;
	dd::EventRelay<KrakenSystem, dd::Events::KrakenAttack> m_EKrakenAttack;
    bool OnPause(const dd::Events::Pause &event);
	bool OnKrakenAppear(const dd::Events::KrakenAppear &event);
	bool OnKrakenAttack(const dd::Events::KrakenAttack &event);
};

}

}

#endif //DAYDREAM_KRAKENSYSTEM_H

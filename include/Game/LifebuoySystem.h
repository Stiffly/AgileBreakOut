//
// Created by Adniklastrator on 2015-09-09.
//

#ifndef DAYDREAM_LIFEBUOYSYSTEM_H
#define DAYDREAM_LIFEBUOYSYSTEM_H

#include <fstream>
#include <iostream>
#include "Core/System.h"
#include "Core/CTransform.h"
#include "Core/CTemplate.h"
#include "Core/EventBroker.h"
#include "Core/World.h"
#include "Physics/EContact.h"
#include "Rendering/CModel.h"
#include "Physics/CPhysics.h"
#include "Physics/CRectangleShape.h"
#include "Game/EPause.h"
#include "Game/EResume.h"
#include "Game/ELifebuoy.h"
#include "Game/ELifebuoyHit.h"
#include "Game/CLifebuoy.h"
#include "Physics/ECreateParticleSequence.h"

namespace dd
{

namespace Systems
{

class LifebuoySystem : public System
{
public:
	LifebuoySystem(World* world, std::shared_ptr<dd::EventBroker> eventBroker)
    : System(world, eventBroker)
    { }

    void Initialize() override;

    void Update(double dt) override;
	void UpdateEntity(double dt, EntityID entity, EntityID parent) override;

	bool IsPaused() const { return m_Pause; }
	void SetPause(const bool& pause) { m_Pause = pause; }

private:
	bool m_Pause = false;
	EntityID m_Template;

	dd::EventRelay<LifebuoySystem, dd::Events::Contact> m_EContact;
	dd::EventRelay<LifebuoySystem, dd::Events::Pause> m_EPause;
	dd::EventRelay<LifebuoySystem, dd::Events::Resume> m_EResume;
	dd::EventRelay<LifebuoySystem, dd::Events::Lifebuoy> m_ELifebuoy;
	dd::EventRelay<LifebuoySystem, dd::Events::LifebuoyHit> m_ELifebuoyHit;

    bool OnContact(const dd::Events::Contact &event);
	bool OnPause(const dd::Events::Pause &event);
	bool OnResume(const dd::Events::Resume &event);
	bool OnLifebuoy(const dd::Events::Lifebuoy &event);
	bool OnLifebuoyHit(const dd::Events::LifebuoyHit &event);

	struct LifebuoyInfo
	{
		EntityID Entity;
		//int Hits = 4;
	};

	float m_LeftEdge = -3.8f;
	float m_RightEdge = 3.8f;
	float m_DownLimit = -5.3f;
	float m_DownEdge = -9.2f;
	std::list<LifebuoyInfo> m_Lifebuoys;
};

}

}

#endif //DAYDREAM_LIFEBUOYSYSTEM_H

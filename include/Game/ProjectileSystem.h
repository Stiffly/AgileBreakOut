//
// Created by Adniklastrator on 2015-09-09.
//

#ifndef DAYDREAM_PROJECTILESYSTEM_H
#define DAYDREAM_PROJECTILESYSTEM_H


#include <fstream>
#include <iostream>

#include "Core/System.h"
#include "Core/CTransform.h"
#include "Core/CTemplate.h"
#include "Core/EventBroker.h"
#include "Core/World.h"
#include "Core/EventBroker.h"
#include "Physics/EContact.h"
#include "Rendering/CModel.h"
#include "Physics/CPhysics.h"
#include "Physics/CCircleShape.h"
#include "Game/EPause.h"
#include "Game/EResume.h"
#include "Game/EInkBlaster.h"
#include "Game/EInkBlasterOver.h"
#include "Game/EActionButton.h"
#include "Game/EHitPad.h"
#include "Game/CProjectile.h"
#include "Game/CBall.h"
#include "Rendering/CSprite.h"
#include "Physics/ECreateParticleSequence.h"

namespace dd
{

namespace Systems
{

class ProjectileSystem : public System
{
public:
	ProjectileSystem(World* world, std::shared_ptr<dd::EventBroker> eventBroker)
    : System(world, eventBroker)
    { }

    void Initialize() override;

    void Update(double dt) override;
	void UpdateEntity(double dt, EntityID entity, EntityID parent) override;

	bool IsPaused() const { return m_Pause; }
	void SetPause(const bool& pause) { m_Pause = pause; }

private:
	bool m_Pause = false;
	bool m_InkBlaster = false;
	bool m_SquidLoaded = false;
	int m_Shots = 0;
	EntityID m_InkBlastTemplate;
	EntityID m_AttachedSquid;

	dd::EventRelay<ProjectileSystem, dd::Events::Contact> m_EContact;
	dd::EventRelay<ProjectileSystem, dd::Events::Pause> m_EPause;
	dd::EventRelay<ProjectileSystem, dd::Events::Resume> m_EResume;
	dd::EventRelay<ProjectileSystem, dd::Events::InkBlaster> m_EInkBlaster;
	dd::EventRelay<ProjectileSystem, dd::Events::HitPad> m_EHitPad;
	dd::EventRelay<ProjectileSystem, dd::Events::ActionButton> m_EActionButton;

    bool OnContact(const dd::Events::Contact &event);
	bool OnPause(const dd::Events::Pause &event);
	bool OnResume(const dd::Events::Resume &event);
	bool OnInkBlaster(const dd::Events::InkBlaster &event);
	bool OnHitPad(const dd::Events::HitPad &event);
	bool OnActionButton(const dd::Events::ActionButton &event);
};

}

}

#endif //DAYDREAM_PROJECTILESYSTEM_H

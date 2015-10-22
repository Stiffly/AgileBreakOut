//
// Created by Adniklastrator on 2015-10-22.
//

#ifndef DAYDREAM_LIFESYSTEM_H
#define DAYDREAM_LIFESYSTEM_H


#include "Core/System.h"
#include "Core/CTransform.h"
#include "Core/CTemplate.h"
#include "Core/EventBroker.h"
#include "Core/World.h"

#include "Core/ConfigFile.h"

#include "Transform/EMove.h"
#include "Rendering/CModel.h"

#include "Game/CLife.h"
#include "Game/EPause.h"
#include "Game/EResume.h"
#include "Game/ELifeLost.h"
#include "Game/EGameOver.h"
#include "Game/EKrakenAttackEnd.h"


namespace dd
{

namespace Systems
{

class LifeSystem : public System
{
public:
    LifeSystem(World* world, std::shared_ptr<dd::EventBroker> eventBroker)
    : System(world, eventBroker)
    { }

    
	void Initialize() override;
	void Update(double dt) override;
	void UpdateEntity(double dt, EntityID entity, EntityID parent) override;

	bool IsPaused() const { return m_Pause; }
	void SetPause(const bool& pause) { m_Pause = pause; }
	int& Lives() { return m_Lives; }
	void SetLives(const int& lives) { m_Lives = lives; }
	int& PastLives() { return m_PastLives; }
	void SetPastLives(const int& pastLives) { m_PastLives = pastLives; }

private:
	bool m_Pause = false;
	int m_Lives = 3;
	int m_PastLives = 3;

	bool m_GodMode = false;

	void CreateLife(int);

    dd::EventRelay<LifeSystem, dd::Events::Pause> m_EPause;
	dd::EventRelay<LifeSystem, dd::Events::Resume> m_EResume;
	dd::EventRelay<LifeSystem, dd::Events::LifeLost> m_ELifeLost;

    bool OnPause(const dd::Events::Pause &event);
	bool OnResume(const dd::Events::Resume &event);
	bool OnLifeLost(const dd::Events::LifeLost &event);
};

}

}

#endif //DAYDREAM_LIFESYSTEM_H

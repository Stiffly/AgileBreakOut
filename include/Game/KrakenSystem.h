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
#include "Game/EPause.h"
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

    dd::EventRelay<KrakenSystem, dd::Events::Pause> m_EPause;
    bool OnPause(const dd::Events::Pause &event);
};

}

}

#endif //DAYDREAM_KRAKENSYSTEM_H

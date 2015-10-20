//
// Created by Adniklastrator on 2015-10-20.
//

#ifndef DAYDREAM_WATERSYSTEM_H
#define DAYDREAM_WATERSYSTEM_H


#include "Core/System.h"
#include "Core/CTransform.h"
#include "Core/CTemplate.h"
#include "Core/EventBroker.h"
#include "Core/World.h"
#include "Transform/EMove.h"
#include "Physics/CWaterVolume.h"
#include "Physics/CRectangleShape.h"
#include "Game/EPause.h"
#include "Game/EResume.h"
#include "Game/ERelevantObjectCreated.h"
#include "Game/ERaiseWater.h"
#include "Game/ERaiseWaterWall.h"
#include "Game/EArrivedAtNewStage.h"
#include "Game/EStageCleared.h"


namespace dd
{

namespace Systems
{

class WaterSystem : public System
{
public:
	WaterSystem(World* world, std::shared_ptr<dd::EventBroker> eventBroker)
    : System(world, eventBroker)
    { }

    
	void Initialize() override;
	void Update(double dt) override;
	void UpdateEntity(double dt, EntityID entity, EntityID parent) override;

	bool IsPaused() const { return m_Pause; }
	void SetPause(const bool& pause) { m_Pause = pause; }

private:
	bool m_Pause = false;
	EntityID m_BottomWall = 0;

    dd::EventRelay<WaterSystem, dd::Events::Pause> m_EPause;
	dd::EventRelay<WaterSystem, dd::Events::Resume> m_EResume;
	dd::EventRelay<WaterSystem, dd::Events::RelevantObjectCreated> m_ERelevantObjectCreated;
	dd::EventRelay<WaterSystem, dd::Events::StageCleared> m_EStageCleared;
	dd::EventRelay<WaterSystem, dd::Events::ArrivedAtNewStage> m_EArrivedAtNewStage;
	dd::EventRelay<WaterSystem, dd::Events::RaiseWater> m_ERaiseWater;
	dd::EventRelay<WaterSystem, dd::Events::RaiseWaterWall> m_ERaiseWaterWall;

    bool OnPause(const dd::Events::Pause &event);
	bool OnResume(const dd::Events::Resume &event);
	bool OnRelevantObjectCreated(const dd::Events::RelevantObjectCreated &event);
	bool OnStageCleared(const dd::Events::StageCleared &event);
	bool OnArrivedAtNewStage(const dd::Events::ArrivedAtNewStage &event);
	bool OnRaiseWater(const dd::Events::RaiseWater &event);
	bool OnRaiseWaterWall(const dd::Events::RaiseWaterWall &event);
};

}

}

#endif //DAYDREAM_WATERSYSTEM_H

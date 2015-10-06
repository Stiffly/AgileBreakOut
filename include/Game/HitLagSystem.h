//
// Created by Adniklastrator on 2015-09-30.
//

#ifndef DAYDREAM_HITLAGSYSTEM_H
#define DAYDREAM_HITLAGSYSTEM_H


#include "Core/System.h"
#include "Core/CTransform.h"
#include "Core/CTemplate.h"
#include "Core/EventBroker.h"
#include "Core/World.h"
#include "Rendering/CSprite.h"
#include "Rendering/CModel.h"
#include "Game/CBrick.h"
#include "Game/CBall.h"
#include "Game/CLife.h"
#include "Game/CPowerUp.h"
#include "Game/EStageCleared.h"
#include "Game/ELifeLost.h"
#include "Game/EResetBall.h"
#include "Game/EScoreEvent.h"
#include "Game/EComboEvent.h"
#include "Game/EMultiBall.h"
#include "Game/EMultiBallLost.h"
#include "Game/EPause.h"
#include "Game/EHitLag.h"
#include "Game/EGameOver.h"
#include "Game/ECreatePowerUp.h"
#include "Game/EPowerUpTaken.h"
#include "Game/Bricks/CPowerUpBrick.h"
#include "Physics/CPhysics.h"
#include "Physics/CCircleShape.h"
#include "Physics/CRectangleShape.h"
#include "Physics/ESetImpulse.h"
#include "Physics/EContact.h"
#include "Sound/CCollisionSound.h"
#include "Game/PadSystem.h"
#include <fstream>
#include <iostream>


namespace dd
{

namespace Systems
{

class HitLagSystem : public System
{
public:
    HitLagSystem(World* world, std::shared_ptr<dd::EventBroker> eventBroker)
    : System(world, eventBroker)
    { }

    void Initialize() override;

    void Update(double dt) override;
    void FlipPause();

    bool IsPaused() const { return m_Pause; }
    void SetPause(const bool& pause) { m_Pause = pause; }
    bool HitLag() const { return m_HitLag; }
    void SetHitLag(const bool& hitLag) { m_HitLag = hitLag; }
    float& HitLagDuration() { return m_HitLagDuration; }
    void SetHitLagDuration(const float& hitLagDuration) { m_HitLagDuration = hitLagDuration; }
    float& HitLagTimer() { return m_HitLagTimer; }
    void SetHitLagTimer(const float& hitLagTimer) { m_HitLagTimer = hitLagTimer; }
    std::string& CurrentType() { return m_CurrentType; }
    void SetCurrentType(const std::string& currentType) { m_CurrentType = currentType; }
private:
    bool m_Pause;
    bool m_HitLag;
    float m_HitLagDuration;
    float m_HitLagTimer;
    std::string m_CurrentType;

    dd::EventRelay<HitLagSystem, dd::Events::Pause> m_EPause;
    dd::EventRelay<HitLagSystem, dd::Events::HitLag> m_EHitLag;

    bool OnPause(const dd::Events::Pause &event);
    bool OnHitLag(const dd::Events::HitLag &event);

};

}

}

#endif //DAYDREAM_HITLAGSYSTEM_H

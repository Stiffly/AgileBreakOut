//
// Created by Adniklastrator on 2015-09-30.
//

#include "PrecompiledHeader.h"
#include "Game/HitLagSystem.h"

void dd::Systems::HitLagSystem::Initialize()
{
    EVENT_SUBSCRIBE_MEMBER(m_EPause, &HitLagSystem::OnPause);
    EVENT_SUBSCRIBE_MEMBER(m_EHitLag, &HitLagSystem::OnHitLag);
}

void dd::Systems::HitLagSystem::Update(double dt)
{
    if (IsPaused()) {
        if (HitLag()) {
            m_HitLagTimer += dt;
            std::cout << m_HitLagTimer << std::endl;
            if (m_HitLagDuration < m_HitLagTimer) {
                m_HitLagTimer = 0;
                SetHitLag(false);
                Events::Pause e;
                EventBroker->Publish(e);
            }
        }
        return;
    }
}

bool dd::Systems::HitLagSystem::OnPause(const dd::Events::Pause &event)
{
    if (IsPaused()) {
        SetPause(false);
    } else {
        SetPause(true);
    }
    return true;
}

bool dd::Systems::HitLagSystem::OnHitLag(const dd::Events::HitLag &event)
{
    SetHitLag(true);
    Events::Pause e;
    EventBroker->Publish(e);
    m_HitLagDuration = event.Time;
    return true;
}
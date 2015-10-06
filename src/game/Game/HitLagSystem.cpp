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
            SetHitLagTimer(HitLagTimer() += dt);
            //std::cout << HitLagTimer() << std::endl;
            if (HitLagDuration() < HitLagTimer()) {
                SetHitLagTimer(0);
                SetHitLag(false);
                FlipPause();
            }
        }
        return;
    }
}

void dd::Systems::HitLagSystem::FlipPause()
{
    Events::Pause e;
    e.Type = CurrentType();
    EventBroker->Publish(e);
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
    if (HitLag()) {
        FlipPause();
    }
    SetHitLag(true);
    SetCurrentType(event.Type);
    FlipPause();
    SetHitLagDuration(event.Time);
    return true;
}
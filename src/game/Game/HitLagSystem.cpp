//
// Created by Adniklastrator on 2015-09-30.
//

#include "PrecompiledHeader.h"
#include "Game/HitLagSystem.h"

void dd::Systems::HitLagSystem::Initialize()
{
    EVENT_SUBSCRIBE_MEMBER(m_EPause, &HitLagSystem::OnPause);
	EVENT_SUBSCRIBE_MEMBER(m_EResume, &HitLagSystem::OnResume);
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
	if (!IsPaused()) {
		Events::Pause e;
		e.Type = CurrentType();
		EventBroker->Publish(e);
	} else {
		Events::Resume e;
		e.Type = CurrentType();
		EventBroker->Publish(e);
	}
}

bool dd::Systems::HitLagSystem::OnPause(const dd::Events::Pause &event)
{
    m_Pause = true;
    return true;
}
bool dd::Systems::HitLagSystem::OnResume(const dd::Events::Resume &event)
{
	m_Pause = false;
	return true;
}

bool dd::Systems::HitLagSystem::OnHitLag(const dd::Events::HitLag &event)
{
    if (HitLag()) {
        FlipPause();
		SetHitLagTimer(0);
    }
    SetHitLag(true);
    SetCurrentType(event.Type);
    FlipPause();
    SetHitLagDuration(event.Time);
    return true;
}
//
// Created by Adniklastrator on 2015-10-15.
//

#include "PrecompiledHeader.h"
#include "Game/KrakenSystem.h"

void dd::Systems::KrakenSystem::Initialize()
{
    EVENT_SUBSCRIBE_MEMBER(m_EPause, &KrakenSystem::OnPause);

	InitializeObjects();
}

void dd::Systems::KrakenSystem::InitializeObjects()
{

}

void dd::Systems::KrakenSystem::UpdateEntity(double dt, EntityID entity, EntityID parent)
{
    if (IsPaused()) {
        return;
    }

}

bool dd::Systems::KrakenSystem::OnPause(const dd::Events::Pause &event)
{
    if (IsPaused()) {
        SetPause(false);
    } else {
        SetPause(true);
    }
    return true;
}

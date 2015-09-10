//
// Created by Adniklastrator on 2015-09-10.
//

#include "PrecompiledHeader.h"
#include "Game/PadSystem.h"
#include "Core/World.h"


void dd::Systems::PadSystem::Initialize()
{
    Events::BindKey r;
    r.KeyCode = GLFW_KEY_RIGHT;
    r.Command = "right";
    EventBroker->Publish(r);

    Events::BindKey l;
    l.KeyCode = GLFW_KEY_LEFT;
    l.Command = "right";
    EventBroker->Publish(l);

    EVENT_SUBSCRIBE_MEMBER(m_EKeyDown, PadSystem::OnKeyDown);

    return;
}

bool dd::Systems::PadSystem::OnKeyDown(const dd::Events::KeyDown &event)
{

    return true;
}

/*bool dd::Systems::PadSteeringInputController::OnCommand(const Events::InputCommand &event)
{
    std::string command = event.Command;

    if (command == "right")
    {

    }
    else if (command == "left")
    {

    }

    return true;
}*/

bool dd::Systems::PadSystem::PadSteeringInputController::OnCommand(const Events::InputCommand &event)
{
    std::string command = event.Command;

    if (command == "right")
    {

    }
    else if (command == "left")
    {

    }

    return true;
}
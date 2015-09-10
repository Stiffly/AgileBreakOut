//
// Created by Adniklastrator on 2015-09-10.
//

#include "PrecompiledHeader.h"
#include "Game/PadSystem.h"
#include "Core/World.h"
#include <iostream>


void dd::Systems::PadSystem::Initialize()
{
    Events::BindKey r;
    r.KeyCode = GLFW_KEY_RIGHT;
    r.Command = "right";
    EventBroker->Publish(r);

    Events::BindKey l;
    l.KeyCode = GLFW_KEY_LEFT;
    l.Command = "left";
    EventBroker->Publish(l);

    ent = m_World->CreateEntity();
	transform = m_World->AddComponent<Components::Transform>(ent);
	transform->Position = glm::vec3(0.f, -5.f, -10.f);
	sprite = m_World->AddComponent<Components::Sprite>(ent);
	sprite->SpriteFile = "Textures/Core/ErrorTexture.png";

    EVENT_SUBSCRIBE_MEMBER(m_EKeyDown, PadSystem::OnKeyDown);
    EVENT_SUBSCRIBE_MEMBER(m_EKeyUp, PadSystem::OnKeyUp);

    return;
}

void dd::Systems::PadSystem::Update(double dt)
{
    if (transform->Velocity.x < -400.f)
    {
        transform->Velocity.x = -400.f;
    }
    else if (transform->Velocity.x > 400.f)
    {
        transform->Velocity.x = 400.f;
    }
    transform->Position += transform->Velocity * (float)dt;
    transform->Velocity += acceleration * (float)dt;
    transform->Velocity -= transform->Velocity * (0.9f * (float)dt);

    if (left)
    {
        acceleration.x = -20.f;
    }
    else if (right)
    {
        acceleration.x = 20.f;
    }
    else
    {
        acceleration.x = 0.f;
    }

    return;
}

bool dd::Systems::PadSystem::OnKeyDown(const dd::Events::KeyDown &event)
{
    int val = event.KeyCode;
    if (val == 265)
    {
        //std::cout << "Up!" << std::endl;
    }
    else if (val == 264)
    {
        //std::cout << "Down!" << std::endl;
    }
    else if (val == 263)
    {
        //std::cout << "Left!" << std::endl;
        //acceleration.x = -0.01f;
        left = true;
    }
    else if (val = 262)
    {
        //std::cout << "Right!" << std::endl;
        //acceleration.x = 0.01f;
        right = true;
    }
    return true;
}

bool dd::Systems::PadSystem::OnKeyUp(const dd::Events::KeyUp &event)
{
    int val = event.KeyCode;
    if (val == 265)
    {

    }
    else if (val == 264)
    {

    }
    else if (val == 263)
    {
        left = false;
    }
    else if (val = 262)
    {
        right = false;
    }
    return true;
}

bool dd::Systems::PadSystem::PadSteeringInputController::OnCommand(const Events::InputCommand &event)
{
    std::string command = event.Command;

    std::cout << "Command!" << std::endl;

    if (command == "right")
    {
        std::cout << "Right!" << std::endl;
    }
    else if (command == "left")
    {
        std::cout << "Left!" << std::endl;
    }

    return true;
}
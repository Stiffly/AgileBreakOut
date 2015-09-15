//
// Created by Adniklastrator on 2015-09-10.
//

#include "PrecompiledHeader.h"
#include "Game/PadSystem.h"
#include "Core/World.h"
#include <iostream>
#include <Game/CPad.h>


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

    EVENT_SUBSCRIBE_MEMBER(m_EKeyDown, PadSystem::OnKeyDown);
    EVENT_SUBSCRIBE_MEMBER(m_EKeyUp, PadSystem::OnKeyUp);
    EVENT_SUBSCRIBE_MEMBER(m_EContact, PadSystem::OnContact);

    return;
}

void dd::Systems::PadSystem::Update(double dt)
{
    if (ent == 0) {
        for (auto it = m_World->GetEntities()->begin(); it != m_World->GetEntities()->end(); it++) {
            if (m_World->GetProperty<std::string>(it->first, "Name") == "Pad") {
                ent = it->first;
                transform = m_World->GetComponent<Components::Transform>(ent);
                break;
            }
        }
    }

    if (transform->Velocity.x < -400.f)
    {
        transform->Velocity.x = -400.f;
    }
    else if (transform->Velocity.x > 400.f)
    {
        transform->Velocity.x = 400.f;
    }
    transform->Position += transform->Velocity * (float)dt;
    transform->Velocity += acceleration  * (float)dt;
    transform->Velocity -= transform->Velocity * (0.9f * (float)dt);
    transform->Position.y = -5.f;
    //transform->Orientation = glm::quat();
    std::cout << transform->Velocity.x << ", " << transform->Velocity.y << std::endl;

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

bool dd::Systems::PadSystem::OnContact(const dd::Events::Contact &event)
{
    EntityID entityBall = event.Entity2;
    auto ball = m_World->GetComponent<Components::Ball>(entityBall);
    if (ball == NULL)
    {
        return false;
    }
    EntityID entityPad = event.Entity1;
    auto pad = m_World->GetComponent<Components::Pad>(entityPad);
    if (pad == NULL)
    {
        return false;
    }
    auto transformBall = m_World->GetComponent<Components::Transform>(entityBall);
    auto transformPad = m_World->GetComponent<Components::Transform>(entityPad);

    float movementMultiplier = 2.f;

    //float movementX = (event.ContactPoint.x - transformPad->Position.x) * movementMultiplier;
    //float movementY = glm::cos((abs(movementX) / (3.2f * movementMultiplier)) * 3.14159265359f / 2) * 2.f;

   // std::cout << movementX << " " << movementY << std::endl;

    float len = glm::length<float>(transformBall->Velocity);

    transformBall->Velocity += glm::vec3(transformPad->Velocity.x, 0, 0);
    transformBall->Velocity = glm::normalize(transformBall->Velocity) * len;


    //transform->Velocity = glm::vec3(movementX, movementY, 0.f);
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
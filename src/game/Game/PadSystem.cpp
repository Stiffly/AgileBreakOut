//
// Created by Adniklastrator on 2015-09-10.
//

#include "PrecompiledHeader.h"
#include "Game/PadSystem.h"
#include "Core/World.h"
#include <iostream>
#include <Game/CPad.h>
#include <Rendering/CPointLight.h>


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
    EVENT_SUBSCRIBE_MEMBER(m_ELifeLost, PadSystem::LifeLost);

    return;
}

void dd::Systems::PadSystem::UpdateEntity(double dt, EntityID entity, EntityID parent)
{
    auto ball = m_World->GetComponent<Components::Ball>(entity);
    if (ball != NULL) {
        if (replaceBall == true) {
            replaceBall = false;

            auto transformEntity = m_World->GetComponent<Components::Transform>(entity);
            transformEntity->Position = glm::vec3(20, 20, -10);

            //Temporary. Create new ball.
            auto ent = m_World->CreateEntity();
            std::shared_ptr<Components::Transform> transform = m_World->AddComponent<Components::Transform>(ent);
            transform->Position = glm::vec3(0.5f, 0.f, -10.f);;
            transform->Scale = glm::vec3(1.f, 1.f, 1.f);
            std::shared_ptr<Components::Sprite> sprite = m_World->AddComponent<Components::Sprite>(ent);
            sprite->SpriteFile = "Textures/Ball.png";
            std::shared_ptr<Components::CircleShape> circleShape = m_World->AddComponent<Components::CircleShape>(ent);
            std::shared_ptr<Components::Ball> cball = m_World->AddComponent<Components::Ball>(ent);
            std::shared_ptr<Components::Physics> physics = m_World->AddComponent<Components::Physics>(ent);
            physics->Static = false;

            m_World->RemoveEntity(entity);
            m_World->CommitEntity(ent);

            Events::SetImpulse e;
            e.Entity = ent;
            e.Impulse = glm::vec2(0.f, -7.f);
            e.Point = glm::vec2(transform->Position.x, transform->Position.y);
            EventBroker->Publish(e);
        }
    }
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

    if (transform->Velocity.x < -maxSpeed)
    {
        transform->Velocity.x = -maxSpeed;
    }
    else if (transform->Velocity.x > maxSpeed)
    {
        transform->Velocity.x = maxSpeed;
    }
    transform->Position += transform->Velocity * (float)dt;
    transform->Velocity += acceleration * (float)dt;
    transform->Velocity -= transform->Velocity * slowdownModifier * (float)dt;

    if (left)
    {
        acceleration.x = -accelerationSpeed;
    }
    else if (right)
    {
        acceleration.x = accelerationSpeed;
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
    else if (val == 262)
    {
        //std::cout << "Right!" << std::endl;
        //acceleration.x = 0.01f;
        right = true;
    }
    else if (val == 82)
    {
        replaceBall = true;
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
    else if (val == 262)
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

    float movementMultiplier = 4.f;

    float whatX = transformBall->Position.x - transformPad->Position.x;
    float movementX;
    if (whatX > 0)
    {
        movementX = movementMultiplier * whatX;
        //std::cout << "Right!" << std::endl;
    }
    else
    {
        movementX = movementMultiplier * whatX;
        //std::cout << "Left!" << std::endl;
    }

    //float movementY = 1;

    //float movementX = (event.ContactPoint.x - transformPad->Position.x) * movementMultiplier;
    float movementY = glm::cos((abs(movementX) / ((1.6f) * movementMultiplier)) * 3.14159265359f / 2) + 1;

    //std::cout << movementX << " " << movementY << std::endl;

    //Temporary solution. Add a new ball and delete the old one.
    auto ent = m_World->CreateEntity();
    std::shared_ptr<Components::Transform> transform = m_World->AddComponent<Components::Transform>(ent);
    transform->Position = glm::vec3(transformBall->Position.x, transformBall->Position.y + 0.01f, -10.f);
    transform->Scale = glm::vec3(0.5f, 0.5f, 0.5f);
    auto model = m_World->AddComponent<Components::Model>(ent);
    model->ModelFile = "Models/Test/Ball/Ballopus.obj";
    //auto pointlight = m_World->AddComponent<Components::PointLight>(ent);
    std::shared_ptr<Components::CircleShape> circleShape = m_World->AddComponent<Components::CircleShape>(ent);
    std::shared_ptr<Components::Ball> cball = m_World->AddComponent<Components::Ball>(ent);
    std::shared_ptr<Components::Physics> physics = m_World->AddComponent<Components::Physics>(ent);
    physics->Static = false;

    m_World->RemoveEntity(entityBall);
    m_World->CommitEntity(ent);

    Events::SetImpulse e;
    e.Entity = ent;
    e.Impulse = glm::vec2(movementX, movementY);
    e.Point = glm::vec2(transform->Position.x, transform->Position.y);
    EventBroker->Publish(e);
}

bool dd::Systems::PadSystem::LifeLost(const dd::Events::LifeLost &event)
{
    replaceBall = true;
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
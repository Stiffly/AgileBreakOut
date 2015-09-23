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
    EVENT_SUBSCRIBE_MEMBER(m_EResetBall, PadSystem::ResetBall);

    return;
}

void dd::Systems::PadSystem::UpdateEntity(double dt, EntityID entity, EntityID parent)
{
    auto ball = m_World->GetComponent<Components::Ball>(entity);
    if (ball != NULL) {
        if (ReplaceBall() == true) {
            SetReplaceBall(false);

            auto transformEntity = m_World->GetComponent<Components::Transform>(entity);
            transformEntity->Position = glm::vec3(20, 20, -10);

            //Temporary. Create new ball.
            /*auto ent = m_World->CreateEntity();
            std::shared_ptr<Components::Transform> transform = m_World->AddComponent<Components::Transform>(ent);
            transform->Position = glm::vec3(0.5f, 0.f, -10.f);
            transform->Scale = glm::vec3(1.f, 1.f, 1.f);
            std::shared_ptr<Components::Sprite> sprite = m_World->AddComponent<Components::Sprite>(ent);
            sprite->SpriteFile = "Textures/Ball.png";
            std::shared_ptr<Components::CircleShape> circleShape = m_World->AddComponent<Components::CircleShape>(ent);
            std::shared_ptr<Components::Ball> cball = m_World->AddComponent<Components::Ball>(ent);
            std::shared_ptr<Components::Physics> physics = m_World->AddComponent<Components::Physics>(ent);
            physics->Static = false;*/

            auto ent = m_World->CreateEntity();
            std::shared_ptr<Components::Transform> transform = m_World->AddComponent<Components::Transform>(ent);
            transform->Position = glm::vec3(0.5f, 0.f, -10.f);
            transform->Scale = glm::vec3(1.f, 1.f, 1.f);
            auto model = m_World->AddComponent<Components::Model>(ent);
            model->ModelFile = "Models/Test/Ball/Ballopus.obj";
            //auto pointlight = m_World->AddComponent<Components::PointLight>(ent);
            std::shared_ptr<Components::CircleShape> circleShape = m_World->AddComponent<Components::CircleShape>(ent);
            std::shared_ptr<Components::Ball> cball = m_World->AddComponent<Components::Ball>(ent);
            cball->Speed = 10.f;
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
    if (Entity() == 0) {
        for (auto it = m_World->GetEntities()->begin(); it != m_World->GetEntities()->end(); it++) {
            if (m_World->GetProperty<std::string>(it->first, "Name") == "Pad") {
                SetEntity(it->first);
                SetTransform(m_World->GetComponent<Components::Transform>(Entity()));
                SetPad(m_World->GetComponent<Components::Pad>(Entity()));
                break;
            }
        }
    }

    auto transform = Transform();
    auto pad = Pad();
    auto acceleration = Acceleration();

    if (transform->Velocity.x < -pad->MaxSpeed) {
        transform->Velocity.x = -pad->MaxSpeed;
    }
    else if (transform->Velocity.x > pad->MaxSpeed) {
        transform->Velocity.x = pad->MaxSpeed;
    }
    transform->Position += transform->Velocity * (float)dt;
    transform->Velocity += acceleration  * (float)dt;
    transform->Velocity -= transform->Velocity * pad->SlowdownModifier * (float)dt;

    if (Left()) {
        acceleration.x = -pad->AccelerationSpeed;
    }
    else if (Right()) {
        acceleration.x = pad->AccelerationSpeed;
    }
    else {
        acceleration.x = 0.f;
    }

    SetTransform(transform);
    SetPad(pad);
    SetAcceleration(acceleration);

    return;
}

bool dd::Systems::PadSystem::OnKeyDown(const dd::Events::KeyDown &event)
{
    int val = event.KeyCode;
    if (val == GLFW_KEY_UP) {
        //std::cout << "Up!" << std::endl;
    } else if (val == GLFW_KEY_DOWN) {
        //std::cout << "Down!" << std::endl;
    } else if (val == GLFW_KEY_LEFT) {
        //std::cout << "Left!" << std::endl;
        //acceleration.x = -0.01f;
        SetLeft(true);
    } else if (val == GLFW_KEY_RIGHT) {
        //std::cout << "Right!" << std::endl;
        //acceleration.x = 0.01f;
        SetRight(true);
    } else if (val == GLFW_KEY_R) {
        SetReplaceBall(true);
    }
    return true;
}

bool dd::Systems::PadSystem::OnKeyUp(const dd::Events::KeyUp &event)
{
    int val = event.KeyCode;
    if (val == GLFW_KEY_UP) {

    } else if (val == GLFW_KEY_DOWN) {

    } else if (val == GLFW_KEY_LEFT) {
        SetLeft(false);
    } else if (val == GLFW_KEY_RIGHT) {
        SetRight(false);
    }
    return true;
}

bool dd::Systems::PadSystem::OnContact(const dd::Events::Contact &event)
{
    /*
    EntityID entityBall = event.Entity2;
    auto ball = m_World->GetComponent<Components::Ball>(entityBall);
    if (ball == NULL) {
        return false;
    }
    EntityID entityPad = event.Entity1;
    auto pad = m_World->GetComponent<Components::Pad>(entityPad);
    if (pad == NULL) {
        return false;
    }
    auto transformBall = m_World->GetComponent<Components::Transform>(entityBall);
    auto transformPad = m_World->GetComponent<Components::Transform>(entityPad);

    float movementMultiplier = 4.f;

    //float movementX = (event.ContactPoint.x - transformPad->Position.x) * movementMultiplier;
    //float movementY = glm::cos((abs(movementX) / (3.2f * movementMultiplier)) * 3.14159265359f / 2) * 2.f;
    if (whatX > 0) {
        movementX = movementMultiplier * whatX;
        //std::cout << "Right!" << std::endl;
    } else {
        movementX = movementMultiplier * whatX;
        //std::cout << "Left!" << std::endl;
    }

   // std::cout << movementX << " " << movementY << std::endl;

    //float movementX = (event.ContactPoint.x - transformPad->Position.x) * movementMultiplier;
    float movementY = glm::cos((abs(movementX) / ((1.6f) * movementMultiplier)) * 3.14159265359f / 2) + 1;

    //std::cout << movementX << " " << movementY << std::endl;

    float len = glm::length<float>(transformBall->Velocity);
    //auto pointlight = m_World->AddComponent<Components::PointLight>(ent);

    transformBall->Velocity += glm::vec3(transformPad->Velocity.x, 0, 0);
    transformBall->Velocity = glm::normalize(transformBall->Velocity) * len;


    //transform->Velocity = glm::vec3(movementX, movementY, 0.f);
     */
}

bool dd::Systems::PadSystem::ResetBall(const dd::Events::ResetBall &event)
{
    SetReplaceBall(true);
    return true;
}

bool dd::Systems::PadSystem::PadSteeringInputController::OnCommand(const Events::InputCommand &event)
{
    std::string command = event.Command;

    std::cout << "Command!" << std::endl;

    if (command == "right") {
        std::cout << "Right!" << std::endl;
    } else if (command == "left") {
        std::cout << "Left!" << std::endl;
    }

    return true;
}
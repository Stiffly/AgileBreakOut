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

    EVENT_SUBSCRIBE_MEMBER(m_EKeyDown, &PadSystem::OnKeyDown);
    EVENT_SUBSCRIBE_MEMBER(m_EKeyUp, &PadSystem::OnKeyUp);
    EVENT_SUBSCRIBE_MEMBER(m_EContact, &PadSystem::OnContact);
    EVENT_SUBSCRIBE_MEMBER(m_EContactPowerUp, &PadSystem::OnContactPowerUp);
    EVENT_SUBSCRIBE_MEMBER(m_EResetBall, &PadSystem::OnResetBall);
    EVENT_SUBSCRIBE_MEMBER(m_EMultiBall, &PadSystem::OnMultiBall);
}

void dd::Systems::PadSystem::UpdateEntity(double dt, EntityID entity, EntityID parent)
{
    auto ball = m_World->GetComponent<Components::Ball>(entity);
    if (ball != NULL) {
        if (ReplaceBall() == true) {
            SetReplaceBall(false);

            auto transformEntity = m_World->GetComponent<Components::Transform>(entity);
            transformEntity->Position = glm::vec3(0, 20, -10);

            m_World->RemoveEntity(entity);

            auto ent = CreateBall();
            auto transform = m_World->GetComponent<Components::Transform>(ent);

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
    transform->Velocity += acceleration * (float)dt;
    transform->Velocity -= transform->Velocity * pad->SlowdownModifier * (float)dt;

    if (Left()) {
        acceleration.x = -pad->AccelerationSpeed;
    } else if (Right()) {
        acceleration.x = pad->AccelerationSpeed;
    } else {
        acceleration.x = 0.f;
    }

    SetTransform(transform);
    SetPad(pad);
    SetAcceleration(acceleration);

    if (MultiBall() == true) {
        SetMultiBall(false);

        Events::MultiBall e;
        e.padTransform = transform;
        EventBroker->Publish(e);
    }

    return;
}

EntityID dd::Systems::PadSystem::CreateBall()
{
    auto ent = m_World->CreateEntity();
    std::shared_ptr<Components::Transform> transform = m_World->AddComponent<Components::Transform>(ent);
    transform->Position = glm::vec3(0.5f, 0.f, -10.f);
    transform->Scale = glm::vec3(1.f, 1.f, 1.f);
    auto model = m_World->AddComponent<Components::Model>(ent);
    model->ModelFile = "Models/Test/Ball/Ballopus.obj";
    //auto pointlight = m_World->AddComponent<Components::PointLight>(ent);
    std::shared_ptr<Components::CircleShape> circleShape = m_World->AddComponent<Components::CircleShape>(ent);
    std::shared_ptr<Components::Ball> cball = m_World->AddComponent<Components::Ball>(ent);
    std::shared_ptr<Components::Physics> physics = m_World->AddComponent<Components::Physics>(ent);
    physics->Static = false;

    m_World->CommitEntity(ent);

    return ent;
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
    } else if (val == GLFW_KEY_M) {
        SetMultiBall(true);
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

    float whatX = transformBall->Position.x - transformPad->Position.x;
    float movementX;
    if (whatX > 0) {
        movementX = movementMultiplier * whatX;
        //std::cout << "Right!" << std::endl;
    } else {
        movementX = movementMultiplier * whatX;
        //std::cout << "Left!" << std::endl;
    }

    //float movementY = 1;

    //float movementX = (event.ContactPoint.x - transformPad->Position.x) * movementMultiplier;
    float movementY = glm::cos((abs(movementX) / ((1.6f) * movementMultiplier)) * 3.14159265359f / 2) + 1;

    //std::cout << movementX << " " << movementY << std::endl;

    //Temporary solution. Add a new ball and delete the old one.
    auto ent = CreateBall();
    auto transform = m_World->GetComponent<Components::Transform>(ent);

    transform->Position = glm::vec3(transformBall->Position.x, transformBall->Position.y + 0.01f, -10.f);

    m_World->RemoveEntity(entityBall);

    Events::SetImpulse e;
    e.Entity = ent;
    e.Impulse = glm::vec2(movementX, movementY);
    e.Point = glm::vec2(transform->Position.x, transform->Position.y);
    EventBroker->Publish(e);
}

bool dd::Systems::PadSystem::OnContactPowerUp(const dd::Events::Contact &event)
{
    EntityID entityPower;
    EntityID entityPad;
    auto powerUp = m_World->GetComponent<Components::PowerUp>(event.Entity1);
    auto pad = m_World->GetComponent<Components::Pad>(event.Entity2);
    if (powerUp != NULL) {
        entityPower = event.Entity1;
    } else {
        powerUp = m_World->GetComponent<Components::PowerUp>(event.Entity2);
        if (powerUp != NULL) {
            entityPower = event.Entity2;
        }
    }
    if (pad != NULL) {
        entityPad = event.Entity2;
    } else {
        pad = m_World->GetComponent<Components::Pad>(event.Entity1);
        if (pad != NULL) {
            entityPad = event.Entity1;
        }
    }

    if (entityPower == NULL || entityPad == NULL) {
        return false;
    }

    pad = m_World->GetComponent<Components::Pad>(entityPad);
    if (pad == NULL) {
        return false;
    }

    m_World->RemoveEntity(entityPower);
    Events::MultiBall e;
    auto transform = m_World->GetComponent<Components::Transform>(entityPad);
    e.padTransform = transform;
    EventBroker->Publish(e);
    return true;
}

bool dd::Systems::PadSystem::OnResetBall(const dd::Events::ResetBall &event)
{
    SetReplaceBall(true);
    return true;
}

bool dd::Systems::PadSystem::OnMultiBall(const dd::Events::MultiBall &event)
{
    auto ent1 = CreateBall();
    auto ent2 = CreateBall();
    auto transform1 = m_World->GetComponent<Components::Transform>(ent1);
    auto transform2 = m_World->GetComponent<Components::Transform>(ent2);
    auto padTransform = event.padTransform;
    float x1 = padTransform->Position.x - 6, x2 = padTransform->Position.x + 6;
    if (x1 < -7) {
        x1 = 7;
    }
    if (x2 > 7) {
        x2 = -7;
    }
    transform1->Position = glm::vec3(x1, -5, -10);
    transform2->Position = glm::vec3(x2, -5, -10);

    Events::SetImpulse e1;
    e1.Entity = ent1;
    e1.Impulse = glm::vec2(6, 6);
    e1.Point = glm::vec2(transform1->Position.x, transform1->Position.y);
    EventBroker->Publish(e1);

    Events::SetImpulse e2;
    e2.Entity = ent2;
    e2.Impulse = glm::vec2(-6, 6);
    e2.Point = glm::vec2(transform2->Position.x, transform2->Position.y);
    EventBroker->Publish(e2);

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
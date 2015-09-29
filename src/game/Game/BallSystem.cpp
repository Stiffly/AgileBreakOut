#include "PrecompiledHeader.h"
#include "Game/BallSystem.h"

dd::Systems::BallSystem::~BallSystem() {

}

void dd::Systems::BallSystem::RegisterComponents(ComponentFactory *cf)
{

}

void dd::Systems::BallSystem::Initialize()
{

    EVENT_SUBSCRIBE_MEMBER(m_Contact, &BallSystem::Contact);
}

void dd::Systems::BallSystem::Update(double dt)
{

}

void dd::Systems::BallSystem::UpdateEntity(double dt, EntityID entity, EntityID parent)
{
    auto ballComponent = m_World->GetComponent<Components::Ball>(entity);
    if (ballComponent != nullptr) {
        auto transform = m_World->GetComponent<Components::Transform>(entity);
        glm::vec2 dir = glm::normalize(glm::vec2(transform->Velocity.x, transform->Velocity.y));
        glm::vec2 up = glm::vec2(0.f, 1.f);
        float angle = glm::acos(glm::dot<float>(dir, up)) * glm::sign(dir.x);
        transform->Orientation = glm::rotate(glm::quat(), angle, glm::vec3(0.f, 0.f, -1.f));
    }
}

void dd::Systems::BallSystem::OnEntityCommit(EntityID entity)
{

}

void dd::Systems::BallSystem::OnEntityRemoved(EntityID entity)
{

}

bool dd::Systems::BallSystem::Contact(const Events::Contact &event)
{
    Components::PowerUp* power1 = m_World->GetComponent<Components::PowerUp>(event.Entity1);
    Components::PowerUp* power2 = m_World->GetComponent<Components::PowerUp>(event.Entity2);

    if (power1 != nullptr || power2 != nullptr) {
        return false;
    }

    Components::Ball* ballComponent1 = m_World->GetComponent<Components::Ball>(event.Entity1);
    Components::Ball* ballComponent2 = m_World->GetComponent<Components::Ball>(event.Entity2);
    Components::Ball* ballComponent;

    if (ballComponent1 != nullptr && ballComponent2 != nullptr) {
        return false;
    }


    EntityID ballEntity, otherEntitiy;

    //Which is the ball?
    if (ballComponent1 != nullptr) {
        ballEntity = event.Entity1;
        otherEntitiy = event.Entity2;
        ballComponent = ballComponent1;
    }
    else if (ballComponent2 != nullptr) {
        ballEntity = event.Entity2;
        otherEntitiy = event.Entity1;
        ballComponent = ballComponent2;
    }
    else {
        return false;
        //TODO: Add support for power-up collisions
    }

    //if this is a brick thats dead do not collide :)

    auto ballTransform = m_World->GetComponent<Components::Transform>(ballEntity);
    glm::vec2 ballVelocity = glm::vec2(ballTransform->Velocity.x, ballTransform->Velocity.y);

    if (m_World->GetProperty<std::string>(otherEntitiy, "Name") == "Pad"){
        auto padTransform = m_World->GetComponent<Components::Transform>(otherEntitiy);
        float x = ballTransform->Position.x - padTransform->Position.x;

        float y = glm::cos((abs(x) / (1.6f)) * glm::pi<float>() / 2.f) + 1.f;

        ballTransform->Velocity = glm::normalize(glm::vec3(x, y ,0.f)) * ballComponent->Speed;
    }
    else {
        glm::vec2 reflectedVelocity = glm::reflect(ballVelocity, event.Normal);
        ballTransform->Velocity = glm::vec3(reflectedVelocity, 0.f);
    }

    return true;
}

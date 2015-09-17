#include "PrecompiledHeader.h"
#include "Game/BallSystem.h"

dd::Systems::BallSystem::~BallSystem() {

}

void dd::Systems::BallSystem::RegisterComponents(ComponentFactory *cf)
{

}

void dd::Systems::BallSystem::Initialize()
{

    EVENT_SUBSCRIBE_MEMBER(m_Contact, BallSystem::Contact);
}

void dd::Systems::BallSystem::Update(double dt)
{

}

void dd::Systems::BallSystem::UpdateEntity(double dt, EntityID entity, EntityID parent)
{

}

void dd::Systems::BallSystem::OnEntityCommit(EntityID entity)
{

}

void dd::Systems::BallSystem::OnEntityRemoved(EntityID entity)
{

}

bool dd::Systems::BallSystem::Contact(const Events::Contact &event)
{
    auto ballComponent1 = m_World->GetComponent<Components::Ball>(event.Entity1);
    auto ballComponent2 = m_World->GetComponent<Components::Ball>(event.Entity2);

    //Which is the ball?
    EntityID ballEntity, otherEntitiy;
    glm::vec2 normal;
    if (ballComponent1 != nullptr) {
        ballEntity = event.Entity1;
        otherEntitiy = event.Entity2;
    }
    else if (ballComponent2 != nullptr) {
        ballEntity = event.Entity2;
        otherEntitiy = event.Entity1;
    }
    else {
        return false;
        //TODO: Add support for power-up collisions
    }

    //normal.x = -normal.x;

    auto ballTransform = m_World->GetComponent<Components::Transform>(ballEntity);
    auto otherTransform = m_World->GetComponent<Components::Transform>(otherEntitiy);

    glm::vec2 ballVelocity = glm::vec2(ballTransform->Velocity.x, ballTransform->Velocity.y);
    glm::vec2 reflectedVelocity = glm::reflect(ballVelocity, event.Normal);
    ballTransform->Velocity = glm::vec3(reflectedVelocity, 0.f);

    //LOG_INFO("Velocity: %f, %f\n", ballTransform->Velocity.x, ballTransform->Velocity.y);


    return true;
}

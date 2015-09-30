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
    EVENT_SUBSCRIBE_MEMBER(m_ELifeLost, &BallSystem::OnLifeLost);
    EVENT_SUBSCRIBE_MEMBER(m_EMultiBallLost, &BallSystem::OnMultiBallLost);
    EVENT_SUBSCRIBE_MEMBER(m_EResetBall, &BallSystem::OnResetBall);
    EVENT_SUBSCRIBE_MEMBER(m_EMultiBall, &BallSystem::OnMultiBall);
    EVENT_SUBSCRIBE_MEMBER(m_EPause, &BallSystem::OnPause);

    //OctoBall
    {
        auto ent = m_World->CreateEntity();
        std::shared_ptr<Components::Transform> transform = m_World->AddComponent<Components::Transform>(ent);
        transform->Position = glm::vec3(-0.f, 50.26f, -10.f);
        transform->Scale = glm::vec3(0.5f, 0.5f, 0.5f);
        transform->Velocity = glm::vec3(0.f, 0.f, 0.f);
        auto model = m_World->AddComponent<Components::Model>(ent);
        model->ModelFile = "Models/Test/Ball/Ballopus.obj";
        std::shared_ptr<Components::CircleShape> circleShape = m_World->AddComponent<Components::CircleShape>(ent);
        std::shared_ptr<Components::Ball> ball = m_World->AddComponent<Components::Ball>(ent);
        ball->Speed = 5.f;
        std::shared_ptr<Components::Physics> physics = m_World->AddComponent<Components::Physics>(ent);
        std::shared_ptr<Components::Template> ballTemplate = m_World->AddComponent<Components::Template>(ent);
        physics->Static = false;
        physics->Category = CollisionLayer::Type::Ball;
        physics->Mask = CollisionLayer::Type::Pad | CollisionLayer::Type::Brick | CollisionLayer::Type::Wall;
        physics->Calculate = true;

        //auto plight = m_World->AddComponent<Components::PointLight>(ent);
        //plight->Radius = 2.f;

        m_World->CommitEntity(ent);

        SetBall(ent);

        auto ent2 = CreateBall();
        auto transform2 = m_World->GetComponent<Components::Transform>(ent2);
        transform2->Position = glm::vec3(-0.f, 0.26f, -10.f);
        transform2->Velocity = glm::vec3(0.0f, -10.f, 0.f);

        m_World->CommitEntity(ent2);
    }

    for (int i = 0; i < Lives(); i++) {
        CreateLife(i);
    }
}

void dd::Systems::BallSystem::Update(double dt)
{
    if (Lives() < 0)
    {
        SetLives(3);
        Events::GameOver e;
        EventBroker->Publish(e);
    }
}

void dd::Systems::BallSystem::UpdateEntity(double dt, EntityID entity, EntityID parent)
{
    if (IsPaused()) {
        return;
    }

    auto templateCheck = m_World->GetComponent<Components::Template>(entity);
    if (templateCheck != nullptr){ return; }
    auto ballComponent = m_World->GetComponent<Components::Ball>(entity);
    if (ballComponent != nullptr) {
        if (ReplaceBall() == true) {
            SetReplaceBall(false);

            auto transform = m_World->GetComponent<Components::Transform>(entity);
            transform->Position = glm::vec3(0.0f, 0.26f, -10.f);
            transform->Velocity = glm::vec3(0.0f, -ballComponent->Speed, 0.f);
        }

        auto transformBall = m_World->GetComponent<Components::Transform>(entity);
        if (glm::abs(transformBall->Velocity.y) < 2) {
            if (transformBall->Velocity.y > 0) {
                transformBall->Velocity.y = 2;
            } else {
                transformBall->Velocity.y = -2;
            }
        }
        if (transformBall->Position.y < -EdgeY() - 4) {
            if (MultiBalls() != 0) {
//                m_World->RemoveComponent<Components::Ball>(entity);
//                m_World->RemoveComponent<Components::Transform>(entity);
//                m_World->RemoveComponent<Components::CircleShape>(entity);
//                m_World->RemoveComponent<Components::Physics>(entity);
                m_World->RemoveEntity(entity);
                Events::MultiBallLost e;
                EventBroker->Publish(e);
            } else if (Lives() == PastLives()) {
                Events::ResetBall be;
                EventBroker->Publish(be);
                Events::LifeLost e;
                e.Entity = entity;
                EventBroker->Publish(e);
                return;
            }
        } else if (transformBall->Position.x > EdgeX()) {
            if (transformBall->Velocity.x > 0) {
                glm::vec2 reflectedVelocity = glm::reflect(glm::vec2(transformBall->Velocity.x, transformBall->Velocity.y), glm::vec2(1, 0));
                transformBall->Velocity = glm::vec3(reflectedVelocity, 0.f);
            }
        } else if (transformBall->Position.x < -EdgeX()) {
            if (transformBall->Velocity.x < 0) {
                glm::vec2 reflectedVelocity = glm::reflect(glm::vec2(transformBall->Velocity.x, transformBall->Velocity.y), glm::vec2(-1, 0));
                transformBall->Velocity = glm::vec3(reflectedVelocity, 0.f);
            }
        } else if (transformBall->Position.y > EdgeY()) {
            if (transformBall->Velocity.y > 0) {
                glm::vec2 reflectedVelocity = glm::reflect(glm::vec2(transformBall->Velocity.x, transformBall->Velocity.y), glm::vec2(0, 1));
                transformBall->Velocity = glm::vec3(reflectedVelocity, 0.f);
            }
        }
    }

    if (Lives() != PastLives()) {
        auto life = m_World->GetComponent<Components::Life>(entity);
        if (life != nullptr) {
            if (life->Number + 1 == PastLives()) {
                m_World->RemoveComponent<Components::Life>(entity);
                m_World->RemoveComponent<Components::Transform>(entity);
                m_World->RemoveEntity(entity);
                SetPastLives(Lives());
            }
        }
    }

    if (ballComponent != nullptr) {
        auto transform = m_World->GetComponent<Components::Transform>(entity);
        glm::vec2 dir = glm::normalize(glm::vec2(transform->Velocity.x, transform->Velocity.y));
        glm::vec2 up = glm::vec2(0.f, 1.f);
        float angle = glm::acos(glm::dot<float>(dir, up)) * glm::sign(dir.x);
        transform->Orientation = glm::rotate(glm::quat(), angle, glm::vec3(0.f, 0.f, -1.f));
    }
}

bool dd::Systems::BallSystem::OnPause(const dd::Events::Pause &event)
{
    if (IsPaused()) {
        SetPause(false);
    } else {
        SetPause(true);
    }
    return true;
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
        float x = (ballTransform->Position.x - padTransform->Position.x) * XMovementMultiplier();

        float y = glm::cos((abs(x) / (1.6f)) * glm::pi<float>() / 2.f) + 1.f;

        ballTransform->Velocity = glm::normalize(glm::vec3(x, y ,0.f)) * ballComponent->Speed;
        ballComponent->Combo = 0;
        Events::ComboEvent ec;
        ec.Combo = ballComponent->Combo;
        ec.Ball = ballEntity;
        EventBroker->Publish(ec);
        //std::cout << "Combo: " << ballComponent->Combo << std::endl;
    }
    else {
        glm::vec2 reflectedVelocity = glm::reflect(ballVelocity, event.Normal);
        ballTransform->Velocity = glm::vec3(reflectedVelocity, 0.f);
    }

    return true;
}

EntityID dd::Systems::BallSystem::CreateBall()
{
    auto ent = m_World->CloneEntity(Ball());

    m_World->RemoveComponent<Components::Template>(ent);

   /* auto ent = m_World->CreateEntity();
    std::shared_ptr<Components::Transform> transform = m_World->AddComponent<Components::Transform>(ent);
    transform->Position = glm::vec3(0.5f, 0.26f, -10.f);
    transform->Scale = glm::vec3(0.5f, 0.5f, 0.5f);
    auto model = m_World->AddComponent<Components::Model>(ent);
    model->ModelFile = "Models/Test/Ball/Ballopus.obj";
    //auto pointlight = m_World->AddComponent<Components::PointLight>(ent);
    std::shared_ptr<Components::CircleShape> circleShape = m_World->AddComponent<Components::CircleShape>(ent);
    std::shared_ptr<Components::Ball> cball = m_World->AddComponent<Components::Ball>(ent);
    std::shared_ptr<Components::Physics> physics = m_World->AddComponent<Components::Physics>(ent);
    physics->Static = false;
    physics->Category = CollisionLayer::Type::Ball;
    physics->Mask = CollisionLayer::Type::Pad | CollisionLayer::Type::Brick | CollisionLayer::Type::Wall;
    physics->Calculate = true;
    cball->Speed = 5.f;

    auto plight = m_World->AddComponent<Components::PointLight>(ent);
    plight->Radius = 2.f;*/

    m_World->CommitEntity(ent);

    return ent;
}

void dd::Systems::BallSystem::CreateLife(int number)
{
    auto life = m_World->CreateEntity();
    std::shared_ptr<Components::Transform> transform = m_World->AddComponent<Components::Transform>(life);
    transform->Position = glm::vec3(-1.5f + number * 0.15f, -2.f, -5.f);
    transform->Scale = glm::vec3(0.1f, 0.1f, 0.1f);

    std::shared_ptr<Components::Life> lifeNr = m_World->AddComponent<Components::Life>(life);
    lifeNr->Number = number;

    auto model = m_World->AddComponent<Components::Model>(life);
    model->ModelFile = "Models/Test/Ball/Ballopus.obj";


    m_World->CommitEntity(life);
}

bool dd::Systems::BallSystem::OnLifeLost(const dd::Events::LifeLost &event)
{
    SetLives(Lives() - 1);

    return true;
}

bool dd::Systems::BallSystem::OnMultiBallLost(const dd::Events::MultiBallLost &event)
{
    SetMultiBalls(MultiBalls()-1);
    return true;
}

bool dd::Systems::BallSystem::OnResetBall(const dd::Events::ResetBall &event)
{
    SetReplaceBall(true);
    return true;
}

bool dd::Systems::BallSystem::OnMultiBall(const dd::Events::MultiBall &event)
{
    auto ent1 = CreateBall();
    auto ent2 = CreateBall();
    auto transform1 = m_World->GetComponent<Components::Transform>(ent1);
    auto transform2 = m_World->GetComponent<Components::Transform>(ent2);
    auto ball1 = m_World->GetComponent<Components::Ball>(ent1);
    auto ball2 = m_World->GetComponent<Components::Ball>(ent2);
    auto padTransform = event.padTransform;
    float x1 = padTransform->Position.x - 2, x2 = padTransform->Position.x + 2;
    if (x1 < -3.1) {
        x1 = 3;
    }
    if (x2 > 3.1) {
        x2 = -3;
    }
    transform1->Position = glm::vec3(x1, -5.5, -10);
    transform2->Position = glm::vec3(x2, -5.5, -10);

    transform1->Velocity = glm::normalize(glm::vec3(5, 5 ,0.f)) * ball1->Speed;
    transform2->Velocity = glm::normalize(glm::vec3(-5, 5 ,0.f)) * ball2->Speed;

    SetMultiBalls(MultiBalls() + 2);

    return true;
}

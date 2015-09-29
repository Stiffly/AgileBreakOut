//
// Created by Adniklastrator on 2015-09-09.
//


#include "PrecompiledHeader.h"
#include "Game/LevelSystem.h"


void dd::Systems::LevelSystem::Initialize()
{
    EVENT_SUBSCRIBE_MEMBER(m_EContact, &LevelSystem::OnContact);
    EVENT_SUBSCRIBE_MEMBER(m_EScoreEvent, &LevelSystem::OnScoreEvent);
    EVENT_SUBSCRIBE_MEMBER(m_EMultiBall, &LevelSystem::OnMultiBall);
    EVENT_SUBSCRIBE_MEMBER(m_ECreatePowerUp, &LevelSystem::OnCreatePowerUp);
    EVENT_SUBSCRIBE_MEMBER(m_EMultiBallLost, &LevelSystem::OnMultiBallLost);
    EVENT_SUBSCRIBE_MEMBER(m_EPowerUpTaken, &LevelSystem::OnPowerUpTaken);
    EVENT_SUBSCRIBE_MEMBER(m_EStageCleared, &LevelSystem::OnStageCleared);

    return;
}

void dd::Systems::LevelSystem::Update(double dt)
{
    if (!m_Initialized) {
        CreateBasicLevel(Rows(), Lines(), SpaceBetweenBricks(), SpaceToEdge());
        m_Initialized = true;
    }
}

void dd::Systems::LevelSystem::UpdateEntity(double dt, EntityID entity, EntityID parent)
{
    auto templateCheck = m_World->GetComponent<Components::Template>(entity);
    if (templateCheck != nullptr){ return; }

    auto ball = m_World->GetComponent<Components::Ball>(entity);
    SetNotResettingTheStage(NotResettingTheStage() + 0.01 * dt);

    if (NumberOfBricks() <= 0 && Restarting() == false) {
        if (MultiBalls() <= 0 && PowerUps() <= 0) {
            if (NotResettingTheStage() > 5) {
                SetRestarting(true);
                Events::ResetBall e;
                EventBroker->Publish(e);
                Events::ScoreEvent es;
                es.Score = 500;
                EventBroker->Publish(es);
                Events::StageCleared ec;
                EventBroker->Publish(ec);
                SetNotResettingTheStage(0);
                //CreateBasicLevel(Rows(), Lines(), SpaceBetweenBricks(), SpaceToEdge());
            }
        } else {
            if (ball != nullptr && MultiBalls() > 0) {
                SetMultiBalls(MultiBalls()-1);
                m_World->RemoveComponent<Components::Ball>(entity);
                m_World->RemoveComponent<Components::Transform>(entity);
                m_World->RemoveComponent<Components::CircleShape>(entity);
                m_World->RemoveComponent<Components::Physics>(entity);
                m_World->RemoveEntity(entity);
            } else {
                auto powerUp = m_World->GetComponent<Components::PowerUp>(entity);
                if (powerUp != nullptr) {
                    SetPowerUps(PowerUps() - 1);
                    m_World->RemoveComponent<Components::PowerUp>(entity);
                    m_World->RemoveComponent<Components::Transform>(entity);
                    m_World->RemoveEntity(entity);
                }
            }
        }
    }
}

void dd::Systems::LevelSystem::CreateBasicLevel(int rows, int lines, glm::vec2 spacesBetweenBricks, float spaceToEdge)
{
    SetNumberOfBricks(rows * lines);
    //std::cout << "You're only doing this once, right?" << std::endl;
    int num = Lines();
    for (int i = 0; i < rows; i++) {
        num--;
        for (int j = 0; j < Lines(); j++) {
            CreateBrick(i, j, spacesBetweenBricks, spaceToEdge, num);
        }
        if (num == 1)
            num = Lines();
    }

    SetNumberOfBricks(rows * lines);
    SetRestarting(false);
    return;
}

void dd::Systems::LevelSystem::CreateBrick(int row, int line, glm::vec2 spacesBetweenBricks, float spaceToEdge, int num)
{
    auto brick = m_World->CreateEntity();
    std::shared_ptr<Components::Transform> transform = m_World->AddComponent<Components::Transform>(brick);
    auto model = m_World->AddComponent<Components::Model>(brick);
    std::shared_ptr<Components::Brick> cBrick = m_World->AddComponent<Components::Brick>(brick);
    std::shared_ptr<Components::RectangleShape> cRec = m_World->AddComponent<Components::RectangleShape>(brick);
    std::shared_ptr<Components::Physics> cPhys = m_World->AddComponent<Components::Physics>(brick);
    cPhys->Static = false;
    cPhys->GravityScale = 0.f;
    cPhys->Category = CollisionLayer::Type::Brick;
    cPhys->Mask = CollisionLayer::Type::Ball;

    /*std::string fileName = "Textures/Bricks/";
    fileName.append(std::to_string(num));
    fileName.append(".png");*/
    //sprite->SpriteFile =  fileName;
    model->ModelFile = "Models/Test/Brick/Brick.obj";
    if ((line == 1 && row == 4) || (line == 3 && row == 2) || (line == 5 && row == 2)) {
        std::shared_ptr<Components::PowerUpBrick> cPow = m_World->AddComponent<Components::PowerUpBrick>(brick);
    }
    float x = line * spacesBetweenBricks.x;
    float y = row * spacesBetweenBricks.y;
    transform->Scale = glm::vec3(0.8, 0.2, 0.2);
    transform->Position = glm::vec3(x - 3, 5 - spaceToEdge - y , -10.f);
    cBrick->Score = 10 * num;

    //sound
    auto collisionSound = m_World->AddComponent<Components::CollisionSound>(brick);
    collisionSound->filePath = "Sounds/Brick/shortbrickbreak.wav";

    m_World->CommitEntity(brick);
    return;
}

void dd::Systems::LevelSystem::ProcessCollision()
{
    // Like, go into brick component and check what it does upon collision. Maybe not done here?
    return;
}

void dd::Systems::LevelSystem::OnEntityRemoved(EntityID entity)
{
    /*auto brick = m_World->GetComponent<Components::Brick>(entity);
    if (brick != nullptr) {
        SetNumberOfBricks(NumberOfBricks() - 1);
        Events::ScoreEvent es;
        es.Score = brick->Score;
        EventBroker->Publish(es);
        if (numberOfBricks < 1) {
            EndLevel();
        }
    }
    return;*/
}

bool dd::Systems::LevelSystem::OnContact(const dd::Events::Contact &event)
{
    EntityID entityBrick;
    EntityID entityBall;
    auto ball = m_World->GetComponent<Components::Ball>(event.Entity2);
    auto brick = m_World->GetComponent<Components::Brick>(event.Entity1);
    if (brick != nullptr) {
        entityBrick = event.Entity1;
    } else {
        brick = m_World->GetComponent<Components::Brick>(event.Entity2);
        if (brick != nullptr) {
            entityBrick = event.Entity2;
        } else {
            return false;
        }
    }
    if (ball != nullptr) {
        entityBall = event.Entity2;
    } else {
        ball = m_World->GetComponent<Components::Ball>(event.Entity1);
        if (ball != nullptr) {
            entityBall = event.Entity1;
        } else {
            return false;
        }
    }

    brick = m_World->GetComponent<Components::Brick>(entityBrick);
    if (brick == nullptr) {
        return false;
    }

    auto power = m_World->GetComponent<Components::PowerUpBrick>(entityBrick);
    if (power != nullptr && NumberOfBricks() > 1) {
        Events::CreatePowerUp e;
        auto transform = m_World->GetComponent<Components::Transform>(entityBrick);
        e.Position = transform->Position;
        EventBroker->Publish(e);
    }

    if (ball != nullptr && Restarting() == false) {
        ball->Combo += 1;
        Events::ComboEvent ec;
        ec.Combo = ball->Combo;
        ec.Ball = entityBall;
        EventBroker->Publish(ec);

        auto ballTransform = m_World->GetComponent<Components::Transform>(entityBall);

//        m_World->RemoveComponent<Components::Brick>(entityBrick);
//        m_World->RemoveComponent<Components::Transform>(entityBrick);
//        m_World->RemoveComponent<Components::RectangleShape>(entityBrick);
//        m_World->RemoveComponent<Components::Physics>(entityBrick);
//        m_World->RemoveEntity(entityBrick);

        auto physicsComponent = m_World->GetComponent<Components::Physics>(entityBrick);
        physicsComponent->GravityScale = 1.f;
        physicsComponent->Mask = CollisionLayer::Type::Water | CollisionLayer::Type::Wall;

        auto transformComponentBrick = m_World->GetComponent<Components::Transform>(entityBrick);
        auto transformComponentBall = m_World->GetComponent<Components::Transform>(entityBall);

        Events::SetImpulse e;
        e.Entity = entityBrick;

        glm::vec2 point = glm::vec2(transformComponentBrick->Position.x + ((transformComponentBall->Position.x - transformComponentBrick->Position.x )/4),
                                    transformComponentBrick->Position.y  + ((transformComponentBall->Position.y - transformComponentBrick->Position.y)/4));

        e.Impulse = glm::normalize(point)*5.f;///2.f;
        e.Point = point;
        EventBroker->Publish(e);
        //TODO: Bricks dont collide with walls D=


        SetNumberOfBricks(NumberOfBricks() - 1);
        if (NumberOfBricks() <= 0) {
            //SetMultiBalls(MultiBalls() + 1);
        }
        Events::ScoreEvent es;
        es.Score = brick->Score * ball->Combo;
        EventBroker->Publish(es);

        std::cout << "Combo: " << ball->Combo << std::endl;
        //std::cout << NumberOfBricks() << std::endl;
        //std::cout << "Brick Score: " << brick->Score  << std::endl;
        std::cout << "Score: " << Score() << std::endl;
    }

    return true;
}

bool dd::Systems::LevelSystem::OnScoreEvent(const dd::Events::ScoreEvent &event)
{
    SetScore(Score() += event.Score);

    return true;
}

bool dd::Systems::LevelSystem::OnMultiBall(const dd::Events::MultiBall &event)
{
    SetMultiBalls(MultiBalls()+2);
    std::cout << MultiBalls() << std::endl;
    return true;
}

bool dd::Systems::LevelSystem::OnMultiBallLost(const dd::Events::MultiBallLost &event)
{
    SetMultiBalls(MultiBalls()-1);
    std::cout << MultiBalls() << std::endl;
    return true;
}

bool dd::Systems::LevelSystem::OnCreatePowerUp(const dd::Events::CreatePowerUp &event)
{
    SetPowerUps(PowerUps() + 1);
    auto powerUp = m_World->CreateEntity();
    std::shared_ptr<Components::Transform> transform = m_World->AddComponent<Components::Transform>(powerUp);
    auto model = m_World->AddComponent<Components::Model>(powerUp);
    std::shared_ptr<Components::CircleShape> cRec = m_World->AddComponent<Components::CircleShape>(powerUp);
    std::shared_ptr<Components::Physics> cPhys = m_World->AddComponent<Components::Physics>(powerUp);
    cPhys->Static = false;
    cPhys->Category = CollisionLayer::Type::PowerUp;
    cPhys->Mask = CollisionLayer::Type::Pad;

    std::shared_ptr<Components::PowerUp> cPow = m_World->AddComponent<Components::PowerUp>(powerUp);


    transform->Position = event.Position;
    transform->Scale = glm::vec3(0.2, 0.2, 0.2);
    transform->Velocity = glm::vec3(0, -4, 0);

    model->ModelFile = "Models/Test/Ball/Ballopus.obj";

    m_World->CommitEntity(powerUp);

    return true;
}

bool dd::Systems::LevelSystem::OnPowerUpTaken(const dd::Events::PowerUpTaken &event)
{
    SetPowerUps(PowerUps() - 1);
    return true;
}

bool dd::Systems::LevelSystem::OnStageCleared(const dd::Events::StageCleared &event)
{
    return true;
}

void dd::Systems::LevelSystem::EndLevel()
{
    Events::StageCleared e;
    EventBroker->Publish(e);
    return;
}


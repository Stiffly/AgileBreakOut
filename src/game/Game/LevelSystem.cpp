//
// Created by Adniklastrator on 2015-09-09.
//


#include "PrecompiledHeader.h"
#include "Game/LevelSystem.h"


void dd::Systems::LevelSystem::Initialize()
{
    EVENT_SUBSCRIBE_MEMBER(m_EContact, LevelSystem::OnContact);
    EVENT_SUBSCRIBE_MEMBER(m_ELifeLost, LevelSystem::OnLifeLost);
    EVENT_SUBSCRIBE_MEMBER(m_EScoreEvent, LevelSystem::OnScoreEvent);
    EVENT_SUBSCRIBE_MEMBER(m_EMultiBall, LevelSystem::OnMultiBall);
    EVENT_SUBSCRIBE_MEMBER(m_ECreatePowerUp, LevelSystem::OnCreatePowerUp);

    for (int i = 0; i < Lives(); i++) {
        CreateLife(i);
    }

    return;
}

void dd::Systems::LevelSystem::CreateLife(int number)
{
    auto life = m_World->CreateEntity();
    std::shared_ptr<Components::Transform> transform = m_World->AddComponent<Components::Transform>(life);
    transform->Position = glm::vec3(-1.5f + number * 0.5f, -2.f, -5.f);
    transform->Scale = glm::vec3(0.1f, 0.1f, 0.1f);

    std::shared_ptr<Components::Life> lifeNr = m_World->AddComponent<Components::Life>(life);
    lifeNr->Number = number;

    auto model = m_World->AddComponent<Components::Model>(life);
    model->ModelFile = "Models/Test/Ball/Ballopus.obj";

    /*std::shared_ptr<Components::Sprite> sprite = m_World->AddComponent<Components::Sprite>(life);
    sprite->SpriteFile = "Textures/Ball.png";*/

    m_World->CommitEntity(life);
}

void dd::Systems::LevelSystem::Update(double dt)
{
    if (!m_Initialized) {
        CreateBasicLevel(Rows(), Lines(), SpaceBetweenBricks(), SpaceToEdge());
        m_Initialized = true;
    }

    if (Lives() < 0)
    {
        SetLives(3);
        Events::GameOver e;
        EventBroker->Publish(e);
    }
}

void dd::Systems::LevelSystem::UpdateEntity(double dt, EntityID entity, EntityID parent)
{
    auto ball = m_World->GetComponent<Components::Ball>(entity);

    if (ball != NULL) {
        auto transformBall = m_World->GetComponent<Components::Transform>(entity);
        if (transformBall->Position.y < -10 || glm::abs(transformBall->Position.x) > 5) {
            if (MultiBalls() != 0) {
                SetMultiBalls(MultiBalls() - 1);
                m_World->RemoveEntity(entity);
            } else if (Lives() == PastLives()) {
                Events::LifeLost e;
                e.Entity = entity;
                EventBroker->Publish(e);
                Events::ResetBall be;
                EventBroker->Publish(be);
                return;
            }
        }
    }

    if (Lives() != PastLives()) {
        auto life = m_World->GetComponent<Components::Life>(entity);
        if (life != NULL) {
            if (life->Number + 1 == PastLives()) {
                m_World->RemoveEntity(entity);
                SetPastLives(Lives());
            }
        }
    }
    if (NumberOfBricks() <= 0) {
        if (MultiBalls() <= 0 && PowerUps() <= 0) {
            Events::ScoreEvent es;
            es.Score = 500;
            EventBroker->Publish(es);
            Events::ResetBall e;
            EventBroker->Publish(e);
            CreateBasicLevel(Rows(), Lines(), SpaceBetweenBricks(), SpaceToEdge());
        } else {
            if (ball != NULL && MultiBalls() > 0) {
                SetMultiBalls(MultiBalls() - 1);
                m_World->RemoveEntity(entity);
            } else {
                auto powerUp = m_World->GetComponent<Components::PowerUp>(entity);
                if (powerUp != NULL) {
                    SetPowerUps(PowerUps() - 1);
                    m_World->RemoveEntity(entity);
                }
            }
        }
    }
}

void dd::Systems::LevelSystem::CreateBasicLevel(int rows, int lines, glm::vec2 spacesBetweenBricks, int spaceToEdge)
{
    int num = 0;
    SetNumberOfBricks(rows * lines);
    for (int i = 0; i < rows; i++) {
        num++;
        for (int j = 0; j < Lines(); j++) {
            CreateBrick(i, j, spacesBetweenBricks, spaceToEdge, num);
        }
        if (num == 7)
            num = 0;
    }

    return;
}

void dd::Systems::LevelSystem::CreateBrick(int row, int line, glm::vec2 spacesBetweenBricks, int spaceToEdge, int num)
{
    auto brick = m_World->CreateEntity();
    std::shared_ptr<Components::Transform> transform = m_World->AddComponent<Components::Transform>(brick);
    //std::shared_ptr<Components::Sprite> sprite = m_World->AddComponent<Components::Sprite>(brick);
    auto model = m_World->AddComponent<Components::Model>(brick);
    std::shared_ptr<Components::Brick> cBrick = m_World->AddComponent<Components::Brick>(brick);
    std::shared_ptr<Components::RectangleShape> cRec = m_World->AddComponent<Components::RectangleShape>(brick);
    std::shared_ptr<Components::Physics> cPhys = m_World->AddComponent<Components::Physics>(brick);
    std::string fileName = "Textures/Bricks/";
    fileName.append(std::to_string(num));
    fileName.append(".png");
    //sprite->SpriteFile =  fileName;
    model->ModelFile = "Models/Test/Brick/Brick.obj";
    if ((line == 1 && row == 4) || (line == 3 && row == 2) || (line == 6 && row == 6)) {
        std::shared_ptr<Components::PowerUpBrick> cPow = m_World->AddComponent<Components::PowerUpBrick>(brick);
    }
    float x = spaceToEdge + line * spacesBetweenBricks.x;
    float y = spaceToEdge + row * spacesBetweenBricks.y;
    transform->Scale = glm::vec3(0.8, 0.2, 0.2);
    transform->Position = glm::vec3(x - 3, y + 3, -10.f);
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
    if (brick != NULL) {
        numberOfBricks--;
        if (numberOfBricks < 1) {
            EndLevel();
        }
    }*/
    return;
}

bool dd::Systems::LevelSystem::OnContact(const dd::Events::Contact &event)
{
    EntityID entityBrick;
    EntityID entityBall;
    auto ball = m_World->GetComponent<Components::Ball>(event.Entity2);
    auto brick = m_World->GetComponent<Components::Brick>(event.Entity1);
    if (brick != NULL) {
        entityBrick = event.Entity1;
    } else {
        brick = m_World->GetComponent<Components::Brick>(event.Entity2);
        if (brick != NULL) {
            entityBrick = event.Entity2;
        } else {
            return false;
        }
    }
    if (ball != NULL) {
        entityBall = event.Entity2;
    } else {
        ball = m_World->GetComponent<Components::Ball>(event.Entity1);
        if (ball != NULL) {
            entityBall = event.Entity1;
        } else {
            return false;
        }
    }

    brick = m_World->GetComponent<Components::Brick>(entityBrick);
    if (brick == NULL) {
        return false;
    }

    auto power = m_World->GetComponent<Components::PowerUpBrick>(entityBrick);
    if (power != NULL) {
        Events::CreatePowerUp e;
        auto transform = m_World->GetComponent<Components::Transform>(entityBrick);
        e.Position = transform->Position;
        EventBroker->Publish(e);
    }

    if (ball != NULL) {
        m_World->RemoveEntity(entityBrick);

        SetNumberOfBricks(NumberOfBricks() - 1);
        Events::ScoreEvent es;
        es.Score = brick->Score;
        EventBroker->Publish(es);

        std::cout << "Score: " << Score() << std::endl;
    }

    return true;
}

bool dd::Systems::LevelSystem::OnLifeLost(const dd::Events::LifeLost &event)
{
    SetLives(Lives() - 1);

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
}

bool dd::Systems::LevelSystem::OnCreatePowerUp(const dd::Events::CreatePowerUp &event)
{
    SetPowerUps(PowerUps() + 1);
    auto powerUp = m_World->CreateEntity();
    std::shared_ptr<Components::Transform> transform = m_World->AddComponent<Components::Transform>(powerUp);
    auto model = m_World->AddComponent<Components::Model>(powerUp);
    std::shared_ptr<Components::CircleShape> cRec = m_World->AddComponent<Components::CircleShape>(powerUp);
    std::shared_ptr<Components::Physics> cPhys = m_World->AddComponent<Components::Physics>(powerUp);
    std::shared_ptr<Components::PowerUp> cPow = m_World->AddComponent<Components::PowerUp>(powerUp);
    cPhys->Static = false;

    transform->Position = event.Position;
    transform->Scale = glm::vec3(0.2, 0.2, 0.2);

    model->ModelFile = "Models/Test/Ball/Ballopus.obj";

    m_World->CommitEntity(powerUp);

    Events::SetImpulse e;
    e.Entity = powerUp;
    e.Impulse = glm::vec2(0, -0.05);
    e.Point = glm::vec2(transform->Position.x, transform->Position.y);
    EventBroker->Publish(e);
    return true;
}

void dd::Systems::LevelSystem::EndLevel()
{
    Events::StageCleared e;
    EventBroker->Publish(e);
    return;
}


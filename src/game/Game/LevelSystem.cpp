//
// Created by Adniklastrator on 2015-09-09.
//


#include "PrecompiledHeader.h"
#include "Game/LevelSystem.h"


void dd::Systems::LevelSystem::Initialize()
{
    EVENT_SUBSCRIBE_MEMBER(m_EContact, LevelSystem::OnContact);
    EVENT_SUBSCRIBE_MEMBER(m_ELifeLost, LevelSystem::LifeLost);
    EVENT_SUBSCRIBE_MEMBER(m_EScoreEvent, LevelSystem::ScoreEvent);

    for (int i = 0; i < Lives(); i++) {
        CreateLife(i);
    }

    return;
}

void dd::Systems::LevelSystem::CreateLife(int number)
{
    auto life = m_World->CreateEntity();
    std::shared_ptr<Components::Transform> transform = m_World->AddComponent<Components::Transform>(life);
    transform->Position = glm::vec3(-4.f + number * 0.5f, -2.f, -5.f);
    transform->Scale = glm::vec3(0.25f, 0.25f, 0.25f);

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
}

void dd::Systems::LevelSystem::UpdateEntity(double dt, EntityID entity, EntityID parent)
{
    auto ball = m_World->GetComponent<Components::Ball>(entity);

    if (ball != NULL) {
        auto transformBall = m_World->GetComponent<Components::Transform>(entity);
        if (transformBall->Position.y < -10) {
            if (Lives() == PastLives()) {
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
    float x = spaceToEdge + line * spacesBetweenBricks.x;
    float y = spaceToEdge + row * spacesBetweenBricks.y;
    transform->Scale = glm::vec3(1.6, 0.35, 0.5);
    transform->Position = glm::vec3(x - 7, y + 1, -10.f);
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
    EntityID entityBrick = event.Entity1;
    auto brick = m_World->GetComponent<Components::Brick>(entityBrick);
    if (brick == NULL) {
        return false;
    }
    EntityID entityBall = event.Entity2;

    m_World->RemoveEntity(entityBrick);

    SetNumberOfBricks(NumberOfBricks() - 1);
    if (NumberOfBricks() == 0) {
        Events::ScoreEvent es;
        es.Score = 500;
        EventBroker->Publish(es);
        Events::ResetBall e;
        EventBroker->Publish(e);
        CreateBasicLevel(Rows(), Lines(), SpaceBetweenBricks(), SpaceToEdge());
    }
    Events::ScoreEvent es;
    es.Score = brick->Score;
    EventBroker->Publish(es);

    //std::cout << "Score: " << Score() << std::endl;

    return true;
}

bool dd::Systems::LevelSystem::LifeLost(const dd::Events::LifeLost &event)
{
    SetLives(Lives() - 1);

    return true;
}

bool dd::Systems::LevelSystem::ScoreEvent(const dd::Events::ScoreEvent &event)
{
    SetScore(Score() += event.Score);

    return true;
}

void dd::Systems::LevelSystem::EndLevel()
{
    Events::StageCleared e;
    EventBroker->Publish(e);
    return;
}


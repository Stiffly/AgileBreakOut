//
// Created by Adniklastrator on 2015-09-09.
//


#include "PrecompiledHeader.h"
#include "Game/LevelSystem.h"


void dd::Systems::LevelSystem::Initialize()
{
    CreateBasicLevel(tRows, tLines, tSpaceBetweenBricks, tSpaceToEdge);

    return;
}

void dd::Systems::LevelSystem::CreateBasicLevel(int rows, int lines, glm::vec2 spacesBetweenBricks, int spaceToEdge)
{
    int num = 0;
    numberOfBricks = rows * lines;
    for (int i = 0; i < rows; i++)
    {
        num++;
        for (int j = 0; j < lines; j++)
        {
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
    std::shared_ptr<Components::Sprite> sprite = m_World->AddComponent<Components::Sprite>(brick);
    std::shared_ptr<Components::Brick> cBrick = m_World->AddComponent<Components::Brick>(brick);
    std::string fileName = "Textures/Bricks/";
    fileName.append(std::to_string(num));
    fileName.append(".png");
    sprite->SpriteFile =  fileName;
    float x = spaceToEdge + line * spacesBetweenBricks.x;
    float y = spaceToEdge + row * spacesBetweenBricks.y;
    transform->Scale = glm::vec3(1.6, 0.4, 0.);
    transform->Position = glm::vec3(x - 7, y + 1, -10.f);
    //sprite->Color = glm::vec4(1.f, 1.f, 1.f, 1.f);
    return;
}

void dd::Systems::LevelSystem::ProcessCollision()
{
    // Like, go into brick component and check what it does upon collision. Maybe not done here?
    return;
}

void dd::Systems::LevelSystem::OnEntityRemoved(EntityID entity)
{
    auto brick = m_World->GetComponent<Components::Brick>(entity);
    if (brick != NULL)
    {
        numberOfBricks--;
        if (numberOfBricks < 1)
        {
            EndLevel();
        }
    }
    return;
}

bool dd::Systems::LevelSystem::OnContact(/*const dd::Events::Contact &event,*/)
{
    /*event.Entity1 - Vi vet inte vilken som.
      event.Entity2 - */
    std::string event = "I'm just a placeholder :(";
    EntityID entityBrick;
    EntityID entityBall;
    EntityID entity1;
    EntityID entity2;

    auto brick = m_World->GetComponent<Components::Brick>(entity1);
    auto ball = m_World->GetComponent<Components::Ball>(entity2);
    if (brick != NULL)
    {
        entityBrick = entity1;
        if (ball != NULL)
        {
            entityBall = entity2;
        }
    }
    else
    {
        brick = m_World->GetComponent<Components::Brick>(entity2);
        if (brick != NULL)
        {
            entityBrick = entity2;
        }

        ball = m_World->GetComponent<Components::Ball>(entity1);
        if (ball != NULL)
        {
            entityBall = entity1;
        }
    }

    if (entityBrick != NULL && entityBall != NULL)
    {
        m_World->RemoveEntity(entityBrick);

        return true;
    }

    return false;
}

void dd::Systems::LevelSystem::EndLevel()
{
    Events::StageCleared e;
    EventBroker->Publish(e);
    return;
}


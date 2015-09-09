//
// Created by Adniklastrator on 2015-09-09.
//


#include "PrecompiledHeader.h"
#include "Game/CBrick.h"
#include "Game/LevelSystem.h"
#include "Core/World.h"
#include "Game/EStageCleared.h"

void dd::Systems::LevelSystem::Initialize()
{
    CreateBasicLevel(tRows, tLines, tSpaceBetweenBricks, tSpaceToEdge);
    return;
}

void dd::Systems::LevelSystem::CreateBasicLevel(int rows, int lines, int spacesBetweenBricks, int spaceToEdge)
{
    numberOfBricks = rows * lines;
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < lines; j++)
        {
            CreateBrick(i, j, spacesBetweenBricks, spaceToEdge);
        }
    }

    return;
}

void dd::Systems::LevelSystem::CreateBrick(int row, int line, int spacesBetweenBricks, int spaceToEdge)
{
    auto brick = m_World->CreateEntity();
    std::shared_ptr<Components::Transform> transform = m_World->AddComponent<Components::Transform>(brick);
    //std::shared_ptr<Components::Sprite> sprite = m_World->AddComponent<Components::Sprite>(brick);
    float x = spaceToEdge + line * spacesBetweenBricks;
    float y = spaceToEdge + line * spacesBetweenBricks;
    transform->Position = glm::vec3(x, y, 0.f);
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

void dd::Systems::LevelSystem::EndLevel()
{
    Events::StageCleared e;
    EventBroker->Publish(e);
    return;
}
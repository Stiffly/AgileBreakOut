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
    std::string fileName = "Textures/Bricks/";
    fileName.append(std::to_string(num));
    fileName.append(".png");
    sprite->SpriteFile =  fileName;
    float x = spaceToEdge + line * spacesBetweenBricks.x;
    float y = spaceToEdge + row * spacesBetweenBricks.y;
    transform->Scale = glm::vec3(0.8, 0.2, 0.);
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

void dd::Systems::LevelSystem::EndLevel()
{
    Events::StageCleared e;
    EventBroker->Publish(e);
    return;
}


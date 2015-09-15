//
// Created by Adniklastrator on 2015-09-09.
//


#include "PrecompiledHeader.h"
#include "Game/LevelSystem.h"


void dd::Systems::LevelSystem::Initialize()
{
    EVENT_SUBSCRIBE_MEMBER(m_EContact, LevelSystem::OnContact);

    return;
}

void dd::Systems::LevelSystem::Update(double dt) {
    if (!m_Initialized) {
        CreateBasicLevel(tRows, tLines, tSpaceBetweenBricks, tSpaceToEdge);
        m_Initialized = true;
    }
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
    transform->Scale = glm::vec3(1, 0.4, 0.2);
    transform->Position = glm::vec3(x - 7, y + 1, -10.f);
    //sprite->Color = glm::vec4(1.f, 1.f, 1.f, 1.f);
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

bool dd::Systems::LevelSystem::OnContact(const dd::Events::Contact &event)
{
    /*event.Entity1 - Vi vet inte vilken som.
      event.Entity2 - */
    EntityID entityBrick = event.Entity1;
    auto brick = m_World->GetComponent<Components::Brick>(entityBrick);
    if (brick == NULL)
    {
        return false;
    }
    EntityID entityBall = event.Entity2;
    //EntityID entity1 = event.Entity1;
    //EntityID entity2 = event.Entity2;

    //auto brick = m_World->GetComponent<Components::Brick>(entity1);
    //auto ball = m_World->GetComponent<Components::Ball>(entity2);
    //std::cout << brick << std::endl;
    //std::cout << ball << std::endl;
    /*if (brick != NULL)
    {
        std::cout << "Entity1 is a brick!" << std::endl;
        entityBrick = entity1;
        if (ball != NULL)
        {
            std::cout << "Entity2 is a ball!" << std::endl;
            entityBall = entity2;
        }
    }
    else
    {
        brick = m_World->GetComponent<Components::Brick>(entity2);
        if (brick != NULL)
        {
            std::cout << "Entity2 is a brick!" << std::endl;
            entityBrick = entity2;
        }

        ball = m_World->GetComponent<Components::Ball>(entity1);
        if (ball != NULL)
        {
            std::cout << "Entity1 is a ball!" << std::endl;
            entityBall = entity1;
        }
    }

    if (entityBrick != NULL && entityBall != NULL)
    {*/
        //std::cout << "We're destroying!" << std::endl;
        m_World->RemoveEntity(entityBrick);

        return true;
    //}

    return false;
}

void dd::Systems::LevelSystem::EndLevel()
{
    Events::StageCleared e;
    EventBroker->Publish(e);
    return;
}


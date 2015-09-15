//
// Created by Adniklastrator on 2015-09-09.
//


#include "PrecompiledHeader.h"
#include "Game/LevelSystem.h"


void dd::Systems::LevelSystem::Initialize()
{
    EVENT_SUBSCRIBE_MEMBER(m_EContact, LevelSystem::OnContact);
    EVENT_SUBSCRIBE_MEMBER(m_EBall, LevelSystem::BallFellOffStage);

    for (int i = 0; i < lives; i++)
    {
        CreateLife(i);
    }

    return;
}

void dd::Systems::LevelSystem::CreateLife(int number)
{
    /*auto life = m_World->CreateEntity();
    std::shared_ptr<Components::Transform> transform = m_World->AddComponent<Components::Transform>(life);
    transform->Position = glm::vec3(-11.f + number, -8.f, -10.f);
    transform->Scale = glm::vec3(0.5f, 0.5f, 0.5f);

    std::shared_ptr<Components::Life> lifeNr = m_World->AddComponent<Components::Life>(life);
    lifeNr->number = number;

    std::shared_ptr<Components::Sprite> sprite = m_World->AddComponent<Components::Sprite>(life);
    sprite->SpriteFile = "Textures/Ball.png";

    m_World->CommitEntity(life);*/
}

void dd::Systems::LevelSystem::Update(double dt)
{
    if (!m_Initialized) {
        CreateBasicLevel(tRows, tLines, tSpaceBetweenBricks, tSpaceToEdge);
        m_Initialized = true;
    }

    if (entityToRemove != NULL)
    {
        //m_World->RemoveEntity(entityToRemove);
        entityToRemove = NULL;
    }
}

void dd::Systems::LevelSystem::UpdateEntity(double dt, EntityID entity, EntityID parent)
{
    auto ball = m_World->GetComponent<Components::Ball>(entity);

    if (ball != NULL)
    {
        auto transformBall = m_World->GetComponent<Components::Transform>(entity);
        if (transformBall->Position.y < -10)
        {
            Events::BallFellOffStage e;
            e.entity = entity;
            EventBroker->Publish(e);
            return;
        }
    }

    if (lives != pastLives)
    {
        auto life = m_World->GetComponent<Components::Life>(entity);
        if (life != NULL)
        {
            if (life->number + 1 == pastLives) {
                pastLives--;
                //m_World->RemoveEntity(entity);
            }
        }
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
    std::shared_ptr<Components::Sprite> sprite = m_World->AddComponent<Components::Sprite>(brick);
    std::shared_ptr<Components::Brick> cBrick = m_World->AddComponent<Components::Brick>(brick);
    std::shared_ptr<Components::RectangleShape> cRec = m_World->AddComponent<Components::RectangleShape>(brick);
    std::shared_ptr<Components::Physics> cPhys = m_World->AddComponent<Components::Physics>(brick);
    std::string fileName = "Textures/Bricks/";
    fileName.append(std::to_string(num));
    fileName.append(".png");
    sprite->SpriteFile =  fileName;
    float x = spaceToEdge + line * spacesBetweenBricks.x;
    float y = spaceToEdge + row * spacesBetweenBricks.y;
    transform->Scale = glm::vec3(1.6, 0.4, 0.);
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
        m_World->RemoveEntity(entityBrick);

        return true;
    //}

    return false;
}

bool dd::Systems::LevelSystem::BallFellOffStage(const dd::Events::BallFellOffStage &event)
{
    /*std::cout << "Did you get here?" << std::endl;
    EntityID entity = event.entity;

    //Temporary. Create new ball.
    auto ent = m_World->CreateEntity();
    std::shared_ptr<Components::Transform> transform = m_World->AddComponent<Components::Transform>(ent);
    transform->Position = glm::vec3(0.5f, 0.f, -10.f);;
    transform->Scale = glm::vec3(1.f, 1.f, 1.f);
    std::shared_ptr<Components::Sprite> sprite = m_World->AddComponent<Components::Sprite>(ent);
    sprite->SpriteFile = "Textures/Ball.png";
    std::shared_ptr<Components::CircleShape> circleShape = m_World->AddComponent<Components::CircleShape>(ent);
    std::shared_ptr<Components::Ball> cball = m_World->AddComponent<Components::Ball>(ent);
    std::shared_ptr<Components::Physics> physics = m_World->AddComponent<Components::Physics>(ent);
    physics->Static = false;

    std::cout << "How about here?" << std::endl;
*/
    //m_World->RemoveEntity(event.entity);
    //entityToRemove = event.entity;
/*    m_World->CommitEntity(ent);

    std::cout << "Or here?" << std::endl;

    Events::SetImpulse e;
    e.Entity = ent;
    e.Impulse = glm::vec2(0.f, -7.f);
    e.Point = glm::vec2(transform->Position.x, transform->Position.y);
    EventBroker->Publish(e);

    std::cout << "Maybe even here?" << std::endl;
*/
    lives--;

    return true;
}

void dd::Systems::LevelSystem::EndLevel()
{
    Events::StageCleared e;
    EventBroker->Publish(e);
    return;
}


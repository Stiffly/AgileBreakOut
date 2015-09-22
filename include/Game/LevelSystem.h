//
// Created by Adniklastrator on 2015-09-09.
//

#ifndef DAYDREAM_LEVELSYSTEM_H
#define DAYDREAM_LEVELSYSTEM_H

#include "Core/System.h"
#include "Core/CTransform.h"
#include "Core/EventBroker.h"
#include "Core/World.h"
#include "Rendering/CSprite.h"
#include "Rendering/CModel.h"
#include "Game/CBrick.h"
#include "Game/CBall.h"
#include "Game/CLife.h"
#include "Game/CPowerUp.h"
#include "Game/EStageCleared.h"
#include "Game/ELifeLost.h"
#include "Game/EResetBall.h"
#include "Game/EScoreEvent.h"
#include "Game/EMultiBall.h"
#include "Game/EGameOver.h"
#include "Game/ECreatePowerUp.h"
#include "Game/Bricks/CPowerUpBrick.h"
#include "Physics/CBoxShape.h"
#include "Physics/CPhysics.h"
#include "Physics/CCircleShape.h"
#include "Physics/ESetImpulse.h"
#include "Physics/EContact.h"
#include "Sound/CCollisionSound.h"
#include "Game/PadSystem.h"
#include <fstream>
#include <iostream>
#include <intrin.h>

namespace dd
{

namespace Systems
{

    // Me trying things out.
class Level
{
public:
    int levelRows;
    int levelLines;
    int levelSpaceBetweenBricks;
    int levelSpaceToEdge;
    int bricks[];
};

class LevelSystem : public System
{
public:
    LevelSystem(World* world, std::shared_ptr<dd::EventBroker> eventBroker)
    : System(world, eventBroker)
    { }

    void Initialize() override;

    void CreateBasicLevel(int, int, glm::vec2, int);
    void CreateLife(int);
    void SaveLevel(int, int, glm::vec2, int); // Shouldn't be here, but I'm experimenting.
    void LoadLevel(char[20]);
    void CreateBrick(int, int, glm::vec2, int, int);
    void ProcessCollision();

    void OnEntityRemoved(EntityID entity);

    void EndLevel();

    void Update(double dt) override;
    void UpdateEntity(double dt, EntityID entity, EntityID parent) override;

    bool Initialized() const { return m_Initialized; }
    void SetInitialized(const bool& initialized) { m_Initialized = initialized; }
    int& Lives() { return m_Lives; }
    void SetLives(const int& lives) { m_Lives = lives; }
    int& PastLives() { return m_PastLives; }
    void SetPastLives(const int& pastLives) { m_PastLives = pastLives; }
    int& MultiBalls() { return m_MultiBalls; }
    void SetMultiBalls(const int& multiBalls) { m_MultiBalls = multiBalls; }
    int& Score() { return m_Score; }
    void SetScore(const int& score) { m_Score = score; }
    int& NumberOfBricks() { return m_NumberOfBricks; }
    void SetNumberOfBricks(const int& numberOfBricks) { m_NumberOfBricks = numberOfBricks; }
    int& Rows() { return m_Rows; }
    void SetRows(const int& rows) { m_Rows = rows; }
    int& Lines() { return m_Lines; }
    void SetLines(const int& lines) { m_Lines = lines; }
    int& SpaceToEdge() { return m_SpaceToEdge; }
    void SetSpaceToEdge(const int& spaceToEdge) { m_SpaceToEdge = spaceToEdge; }

    glm::vec2& SpaceBetweenBricks() { return m_SpaceBetweenBricks; }
    void SetSpaceBetweenBricks(const glm::vec2& spaceBetweenBricks) { m_SpaceBetweenBricks = spaceBetweenBricks; }

private:
    bool m_Initialized = false;
    int m_Lives = 3;
    int m_PastLives = 3;
    int m_MultiBalls = 0;
    int m_Score = 0;
    int m_NumberOfBricks;
    int m_Rows = 7;
    int m_Lines = 8;
    int m_SpaceToEdge = 0;
    glm::vec2 m_SpaceBetweenBricks = glm::vec2(2, 0.5);

    dd::EventRelay<LevelSystem, dd::Events::Contact> m_EContact;
    dd::EventRelay<LevelSystem, dd::Events::LifeLost> m_ELifeLost;
    dd::EventRelay<LevelSystem, dd::Events::ScoreEvent> m_EScoreEvent;
    dd::EventRelay<LevelSystem, dd::Events::MultiBall> m_EMultiBall;
    dd::EventRelay<LevelSystem, dd::Events::CreatePowerUp> m_ECreatePowerUp;

    bool OnContact(const dd::Events::Contact &event);
    bool OnLifeLost(const dd::Events::LifeLost &event);
    bool OnScoreEvent(const dd::Events::ScoreEvent &event);
    bool OnMultiBall(const dd::Events::MultiBall &event);
    bool OnCreatePowerUp(const dd::Events::CreatePowerUp &event);
};

}

}

#endif //DAYDREAM_LEVELSYSTEM_H

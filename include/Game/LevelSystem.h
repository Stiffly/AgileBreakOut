//
// Created by Adniklastrator on 2015-09-09.
//

#ifndef DAYDREAM_LEVELSYSTEM_H
#define DAYDREAM_LEVELSYSTEM_H

#include "Core/System.h"
#include "Core/CTransform.h"
#include "Core/CTemplate.h"
#include "Core/EventBroker.h"
#include "Core/World.h"
#include "Rendering/CSprite.h"
#include "Rendering/CModel.h"
#include "Game/CBrick.h"
#include "Game/CBall.h"
#include "Game/CLife.h"
#include "Game/CProjectile.h"
#include "Game/CPowerUp.h"
#include "Game/EStageCleared.h"
#include "Game/ELifeLost.h"
#include "Game/EResetBall.h"
#include "Game/EScoreEvent.h"
#include "Game/EComboEvent.h"
#include "Game/EHitPad.h"
#include "Game/ELifebuoy.h"
#include "Game/EStickyPad.h"
#include "Game/EInkBlaster.h"
#include "Game/EMultiBall.h"
#include "Game/EMultiBallLost.h"
#include "Game/EPause.h"
#include "Game/EGameOver.h"
#include "Game/ECreatePowerUp.h"
#include "Game/EPowerUpTaken.h"
//#include "Game/Bricks/CPowerUpBrick.h"
#include "Physics/CPhysics.h"
#include "Physics/CCircleShape.h"
#include "Physics/CRectangleShape.h"
#include "Physics/ESetImpulse.h"
#include "Physics/EContact.h"
#include "Sound/CCollisionSound.h"
#include "Game/PadSystem.h"
#include <fstream>
#include <iostream>

#include "Physics/ECreateParticleSequence.h"

namespace dd
{

namespace Systems
{

    // Me trying things out.
/*class Level
{
public:
    int levelRows = 6;
    int levelLines = 7;
    glm::vec2 levelSpaceBetweenBricks = glm::vec2(1, 0.4);
    float levelSpaceToEdge = 0.25f;
    std::array<int, 42> bricks;
};*/

class LevelSystem : public System
{
public:
    LevelSystem(World* world, std::shared_ptr<dd::EventBroker> eventBroker)
    : System(world, eventBroker)
    { }

    void Initialize() override;

    void CreateBasicLevel(int, int, glm::vec2, float);
    void CreateLevel(int);
    void CreateBrick(int, int, glm::vec2, float, int, int, int);

    void OnEntityRemoved(EntityID entity);

    void EndLevel();

    void Update(double dt) override;
    void UpdateEntity(double dt, EntityID entity, EntityID parent) override;

    bool Restarting() const { return m_Restarting; }
    void SetRestarting(const bool& restarting) { m_Restarting = restarting; }
    bool Initialized() const { return m_Initialized; }
    void SetInitialized(const bool& initialized) { m_Initialized = initialized; }
    bool IsPaused() const { return m_Pause; }
    void SetPause(const bool& pause) { m_Pause = pause; }
    int& MultiBalls() { return m_MultiBalls; }
    void SetMultiBalls(const int& multiBalls) { m_MultiBalls = multiBalls; }
    int& PowerUps() { return m_PowerUps; }
    void SetPowerUps(const int& powerUps) { m_PowerUps = powerUps; }
    int& Score() { return m_Score; }
    void SetScore(const int& score) { m_Score = score; }
    int& NumberOfBricks() { return m_NumberOfBricks; }
    void SetNumberOfBricks(const int& numberOfBricks) { m_NumberOfBricks = numberOfBricks; }
    int& Rows() { return m_Rows; }
    void SetRows(const int& rows) { m_Rows = rows; }
    int& Lines() { return m_Lines; }
    void SetLines(const int& lines) { m_Lines = lines; }
    float& SpaceToEdge() { return m_SpaceToEdge; }
    void SetSpaceToEdge(const float& spaceToEdge) { m_SpaceToEdge = spaceToEdge; }

    glm::vec2& SpaceBetweenBricks() { return m_SpaceBetweenBricks; }
    void SetSpaceBetweenBricks(const glm::vec2& spaceBetweenBricks) { m_SpaceBetweenBricks = spaceBetweenBricks; }
    float& NotResettingTheStage() { return m_NotResettingTheStage; }
    void SetNotResettingTheStage(const float& notResettingTheStage) { m_NotResettingTheStage = notResettingTheStage; }

private:
    bool m_Cleared = false;
    bool m_Restarting = false;
    bool m_Initialized = false;
    bool m_Pause = false;
    int m_LooseBricks = 0;
    int m_CurrentCluster = 1;
    int m_CurrentLevel = 1;
    int m_MultiBalls = 0;
    int m_PowerUps = 0;
    int m_Score = 0;
    int m_NumberOfBricks;
    int m_Rows = 6;
    int m_Lines = 7;
    float m_SpaceToEdge = 0.25f;
    glm::vec2 m_SpaceBetweenBricks = glm::vec2(1, 0.4);
    float m_NotResettingTheStage = 5.f;

    const int EmptyBrickSpace = 0;
    const int StandardBrick = 1;
    const int MultiBallBrick = 2;
	const int LifebuoyBrick = 3;
	const int StickyBrick = 4;
	const int InkBlasterBrick = 5;

    EntityID m_BrickTemplate;

    std::array<int, 42> m_Bricks;

    dd::EventRelay<LevelSystem, dd::Events::Contact> m_EContact;
    dd::EventRelay<LevelSystem, dd::Events::ScoreEvent> m_EScoreEvent;
    dd::EventRelay<LevelSystem, dd::Events::MultiBall> m_EMultiBall;
    dd::EventRelay<LevelSystem, dd::Events::MultiBallLost> m_EMultiBallLost;
    dd::EventRelay<LevelSystem, dd::Events::CreatePowerUp> m_ECreatePowerUp;
    dd::EventRelay<LevelSystem, dd::Events::PowerUpTaken> m_EPowerUpTaken;
    dd::EventRelay<LevelSystem, dd::Events::StageCleared> m_EStageCleared;
    dd::EventRelay<LevelSystem, dd::Events::Pause> m_EPause;
    dd::EventRelay<LevelSystem, dd::Events::HitPad> m_EHitPad;

    bool OnContact(const dd::Events::Contact &event);
    bool OnScoreEvent(const dd::Events::ScoreEvent &event);
    bool OnMultiBall(const dd::Events::MultiBall &event);
    bool OnMultiBallLost(const dd::Events::MultiBallLost &event);
    bool OnCreatePowerUp(const dd::Events::CreatePowerUp &event);
    bool OnPowerUpTaken(const dd::Events::PowerUpTaken &event);
    bool OnStageCleared(const dd::Events::StageCleared &event);
    bool OnPause(const dd::Events::Pause &event);
    bool OnHitPad(const dd::Events::HitPad &event);

    void GetNextLevel();
};

}

}

#endif //DAYDREAM_LEVELSYSTEM_H

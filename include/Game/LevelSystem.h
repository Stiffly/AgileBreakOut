//
// Created by Adniklastrator on 2015-09-09.
//

#ifndef DAYDREAM_LEVELSYSTEM_H
#define DAYDREAM_LEVELSYSTEM_H

#include "Core/System.h"
#include "Core/CTransform.h"
#include "Rendering/CSprite.h"
#include "Game/CBrick.h"
#include "Core/EventBroker.h"
#include "Core/World.h"
#include "Game/CBall.h"
#include "Game/CLife.h"
#include "Game/EStageCleared.h"
#include "Game/ELifeLost.h"
#include "Physics/CBoxShape.h"
#include "Physics/CPhysics.h"
#include "Physics/CCircleShape.h"
#include "Physics/ESetImpulse.h"
#include "Physics/EContact.h"
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
    int lives = 3;
    int pastLives = 3;

private:
    dd::EventRelay<LevelSystem, dd::Events::Contact> m_EContact;
    dd::EventRelay<LevelSystem, dd::Events::LifeLost> m_ELifeLost;

    bool OnContact(const dd::Events::Contact &event);
    bool LifeLost(const dd::Events::LifeLost &event);

    bool m_Initialized = false;

    int numberOfBricks;
    int tRows = 7;
    int tLines = 8;
    glm::vec2 tSpaceBetweenBricks = glm::vec2(2, 0.5);
    int tSpaceToEdge = 0;

    EntityID entityToRemove = NULL;
};

}

}

#endif //DAYDREAM_LEVELSYSTEM_H

//
// Created by Adniklastrator on 2015-09-09.
//

#ifndef DAYDREAM_LEVELSYSTEM_H
#define DAYDREAM_LEVELSYSTEM_H

#include "Core/System.h"
#include "Core/CTransform.h"
#include "Rendering/CSprite.h"
#include "Game/CBrick.h"
//#include "Core/EContact.h"
#include "Core/EventBroker.h"
#include "Core/World.h"
#include "Game/EStageCleared.h"
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
    void SaveLevel(int, int, glm::vec2, int); // Shouldn't be here, but I'm experimenting.
    void LoadLevel(char[20]);
    void CreateBrick(int, int, glm::vec2, int, int);
    void ProcessCollision();

    void OnEntityRemoved(EntityID entity);

    void EndLevel();

private:
    //dd::EventRelay<LevelSystem, dd::Events::Contact> m_EContact;

    bool OnContact(/*const dd::Events::Contact &event*/);

    int numberOfBricks;
    int tRows = 7;
    int tLines = 8;
    glm::vec2 tSpaceBetweenBricks = glm::vec2(2, 0.5);
    int tSpaceToEdge = 0;
};

}

}

#endif //DAYDREAM_LEVELSYSTEM_H

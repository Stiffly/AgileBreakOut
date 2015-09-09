//
// Created by Adniklastrator on 2015-09-09.
//

#ifndef DAYDREAM_LEVELSYSTEM_H
#define DAYDREAM_LEVELSYSTEM_H

#include "Core/System.h"
#include "Core/CTransform.h"
//#include "Rendering/CSprite.h"
#include "Core/EventBroker.h"

namespace dd
{

namespace Systems
{

class LevelSystem : public System
{
public:
    LevelSystem(World* world, std::shared_ptr<dd::EventBroker> eventBroker)
    : System(world, eventBroker)
    { }

    void Initialize() override;

    void CreateBasicLevel(int, int, int, int);
    void CreateBrick(int, int, int, int);
    void ProcessCollision();

    void OnEntityRemoved(EntityID entity);

    void EndLevel();



private:
    int numberOfBricks;
    EntityID bricks[];
    int tRows = 8;
    int tLines = 8;
    int tSpaceBetweenBricks = 30;
    int tSpaceToEdge = 30;
};

}

}

#endif //DAYDREAM_LEVELSYSTEM_H

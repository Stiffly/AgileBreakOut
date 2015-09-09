#ifndef DAYDREAM_PHYSICSSYSTEM_H
#define DAYDREAM_PHYSICSSYSTEM_H

#include "Core/System.h"
#include <Box2D/Box2D.h>

namespace dd
{

namespace Systems {


class PhysicsSystem : public System {
    PhysicsSystem(World* world, std::shared_ptr<dd::EventBroker> eventBroker)
    : System(world, eventBroker) {}

    ~PhysicsSystem();

    void Initialize();

    // Called once per system every tick
    void Update(double dt);
    // Called once for every entity in the world every tick
    void UpdateEntity(double dt, EntityID entity, EntityID parent);

    // Called when components are committed to an entity
    void OnEntityCommit(EntityID entity);

    // Called when an entity is removed
    void OnEntityRemoved(EntityID entity);


    b2Vec2 m_Gravity;
    b2World* m_PhysicsWorld;
    float m_TimeStep;
    int m_VelocityIterations, m_PositionIterations;
};

}
}

#endif //DAYDREAM_PHYSICSSYSTEM_H

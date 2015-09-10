#ifndef DAYDREAM_PHYSICSSYSTEM_H
#define DAYDREAM_PHYSICSSYSTEM_H

#include <unordered_map>

#include "Core/System.h"
#include "Core/World.h"
#include "Core/CBoxShape.h"
#include "Core/Physics/CPhysics.h"
#include <Box2D/Box2D.h>
#include <Core/CTransform.h>


namespace dd
{

namespace Systems
{

//KOLLA http://box2d.org/manual.pdf p52 Pre-Solve Event

class PhysicsSystem : public System
{
public:
    PhysicsSystem(World* world, std::shared_ptr<dd::EventBroker> eventBroker)
    : System(world, eventBroker) {}

    void RegisterComponents(ComponentFactory* cf) override;
    void Initialize() override;

    // Called once per system every tick
    void Update(double dt) override;
    // Called once for every entity in the world every tick
    void UpdateEntity(double dt, EntityID entity, EntityID parent) override;

    // Called when components are committed to an entity
    void OnEntityCommit(EntityID entity) override;

    // Called when an entity is removed
    void OnEntityRemoved(EntityID entity) override;

private:



    b2Vec2 m_Gravity;
    b2World* m_PhysicsWorld;
    float m_TimeStep;
    int m_VelocityIterations, m_PositionIterations;

    std::unordered_map<EntityID, b2Body*> m_Bodies;

    void CreateBody(EntityID entity);
};

}
}

#endif //DAYDREAM_PHYSICSSYSTEM_H

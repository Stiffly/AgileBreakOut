#ifndef DAYDREAM_PHYSICSSYSTEM_H
#define DAYDREAM_PHYSICSSYSTEM_H

#include <unordered_map>

#include "Core/System.h"
#include "Core/World.h"
#include "Physics/CBoxShape.h"
#include "Physics/CPhysics.h"
#include <Box2D/Box2D.h>
#include "Core/CTransform.h"
#include "Transform/TransformSystem.h"
#include "Physics/EContact.h"
#include "Physics/ESetImpulse.h"
#include "Physics/CCircleShape.h"
#include "Core/EventBroker.h"


namespace dd
{

namespace Systems
{

//KOLLA http://box2d.org/manual.pdf p52 Pre-Solve Event

class PhysicsSystem : public System
{
    friend class ContractListener;

public:
    PhysicsSystem(World* world, std::shared_ptr<dd::EventBroker> eventBroker)
    : System(world, eventBroker) {}

    ~PhysicsSystem();

    EventRelay<PhysicsSystem, Events::SetImpulse> m_SetImpulse;
    bool SetImpulse(const Events::SetImpulse &event);

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
    float m_Accumulator;

    int m_VelocityIterations, m_PositionIterations;

    std::unordered_map<EntityID, b2Body*> m_EntitiesToBodies;
    std::unordered_map<b2Body*, EntityID> m_BodiesToEntities;

    void CreateBody(EntityID entity);


    struct Impulse
    {
        b2Body* Body;
        b2Vec2 Impulse;
        b2Vec2 Point;
    };
    std::list<Impulse> m_Impulses;



    class ContactListener : public b2ContactListener
    {
    public:
        ContactListener(PhysicsSystem* physicsSystem)
                : m_PhysicsSystem(physicsSystem) { }

        void BeginContact(b2Contact* contact)
        {
            b2Vec2 contactPoint = contact->GetManifold()->localPoint;

            Events::Contact e;
            e.ContactPoint = glm::vec2(contactPoint.x, contactPoint.y);
            e.Entity1 = m_PhysicsSystem->m_BodiesToEntities[contact->GetFixtureA()->GetBody()];
            e.Entity2 = m_PhysicsSystem->m_BodiesToEntities[contact->GetFixtureB()->GetBody()];

            m_PhysicsSystem->EventBroker->Publish(e);
        }
        void EndContact(b2Contact* contact)
        { /* handle end event */ }
        void PreSolve(b2Contact* contact, const b2Manifold* oldManifold)
        { /* handle pre-solve event */ }
        void PostSolve(b2Contact* contact, const b2ContactImpulse* impulse)
        { /* handle post-solve event */ }

    private:
        PhysicsSystem* m_PhysicsSystem;
    };

    ContactListener* m_ContactListener;
};

}
}

#endif //DAYDREAM_PHYSICSSYSTEM_H

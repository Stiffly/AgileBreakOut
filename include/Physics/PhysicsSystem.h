#ifndef DAYDREAM_PHYSICSSYSTEM_H
#define DAYDREAM_PHYSICSSYSTEM_H

#include <unordered_map>

#include <Box2D/Box2D.h>

#include "Core/System.h"
#include "Core/World.h"
#include "Core/EventBroker.h"
#include "Core/CTransform.h"
#include "Transform/TransformSystem.h"
#include "CRectangleShape.h"
#include "Physics/CPhysics.h"
#include "Physics/EContact.h"
#include "Physics/ESetImpulse.h"
#include "Physics/CCircleShape.h"
#include "Physics/CWaterVolume.h"
#include "Game/CPad.h"
#include "Game/CBall.h"
#include "Rendering/CSprite.h"


namespace dd
{

namespace Systems
{

class PhysicsSystem : public System
{
    friend class ContractListener;

public:
    PhysicsSystem(World* world, std::shared_ptr<dd::EventBroker> eventBroker)
    : System(world, eventBroker) { }

    ~PhysicsSystem();

    void RegisterComponents(ComponentFactory* cf) override;
    void Initialize() override;
    void Update(double dt) override;
    void UpdateEntity(double dt, EntityID entity, EntityID parent) override;
    void OnEntityCommit(EntityID entity) override;
    void OnEntityRemoved(EntityID entity) override;

private:
    struct Impulse
    {
        b2Body* Body;
        b2Vec2 Impulse;
        b2Vec2 Point;
    };

    b2Vec2 m_Gravity = b2Vec2(0.f, -9.82f);
    b2World* m_PhysicsWorld = nullptr;
    float m_TimeStep = 1.f/60.f;
    float m_Accumulator = 0.f;
    int m_VelocityIterations = 6;
    int m_PositionIterations = 2;

    b2ParticleSystem *m_ParticleSystem;
    b2ParticleGroup* t_watergroup;

    std::unordered_map<EntityID, b2Body*> m_EntitiesToBodies;
    std::unordered_map<b2Body*, EntityID> m_BodiesToEntities;
    std::unordered_map<EntityID, const b2ParticleHandle*> m_EntitiesToParticleHandle;
    std::unordered_map<const b2ParticleHandle*, EntityID> m_ParticleHandleToEntities;

    std::list<Impulse> m_Impulses;

    void CreateBody(EntityID entity);
    void InitializeWater();
    void SyncWater(); //TODO: Probably remove this
    void CreateParticleGroup(EntityID entity);

    EventRelay<PhysicsSystem, Events::SetImpulse> m_SetImpulse;
    bool SetImpulse(const Events::SetImpulse &event);

    class ContactListener : public b2ContactListener
    {
    public:
        ContactListener(PhysicsSystem* physicsSystem)
                : m_PhysicsSystem(physicsSystem) { }

        void BeginContact(b2Contact* contact);
        void EndContact(b2Contact* contact);
        void PreSolve(b2Contact* contact, const b2Manifold* oldManifold);
        void PostSolve(b2Contact* contact, const b2ContactImpulse* impulse);

    private:
        PhysicsSystem* m_PhysicsSystem;
    };

    ContactListener* m_ContactListener;
};

}
}

#endif //DAYDREAM_PHYSICSSYSTEM_H

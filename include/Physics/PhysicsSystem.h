#ifndef DAYDREAM_PHYSICSSYSTEM_H
#define DAYDREAM_PHYSICSSYSTEM_H

#include <unordered_map>

#include <Box2D/Box2D.h>

#include "Core/System.h"
#include "Core/World.h"
#include "Core/EventBroker.h"
#include "Core/CTemplate.h"

#include "Physics/CRectangleShape.h"
#include "Physics/CCircleShape.h"
#include "Physics/CPhysics.h"
#include "Physics/CWaterVolume.h"
#include "Core/CTransform.h"
#include "Transform/TransformSystem.h"
#include "CRectangleShape.h"
#include "Physics/CPhysics.h"
#include "Physics/EContact.h"
#include "Physics/ESetImpulse.h"
#include "Physics/CParticle.h"
#include "Physics/CCircleShape.h"
#include "Physics/CParticleEmitter.h"
#include "Game/CPad.h"
#include "Game/CPad.h"
#include "Game/CBall.h"
#include "Transform/TransformSystem.h"
#include "Rendering/CSprite.h"
#include "Core/CTemplate.h"

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
            bool m_Pause = false;


            b2Vec2 m_Gravity = b2Vec2(0.f, -9.82f);
            b2World* m_PhysicsWorld = nullptr;
            float m_TimeStep = 1.f/60.f;
            float m_Accumulator = 0.f;
            int m_VelocityIterations = 6;
            int m_PositionIterations = 2;

            std::vector<b2ParticleSystem*> m_ParticleSystem;
            std::vector<b2ParticleGroup*> t_ParticleGroup;

            std::vector<std::unordered_map<EntityID, const b2ParticleHandle*>> m_EntitiesToParticleHandle;
            std::vector<std::unordered_map<const b2ParticleHandle*, EntityID>> m_ParticleHandleToEntities;
            std::unordered_map<EntityID, b2Body*> m_EntitiesToBodies;
            std::unordered_map<b2Body*, EntityID> m_BodiesToEntities;



            void CreateBody(EntityID entity);
            void SyncEntitiesWithBodies();
            void SyncBodiesWithEntities();
            void InitializeWater();
            void CreateParticleGroup(EntityID entity);

            b2ParticleSystem* CreateParticleSystem(float radius, float gravityScale);
            void CreateParticleEmitter(EntityID entity);
            void UpdateParticleEmitters(double dt); //TODO: Remove them and particles if needed.

            EventRelay<PhysicsSystem, Events::SetImpulse> m_SetImpulse;
            bool SetImpulse(const Events::SetImpulse &event);

            //TODO: Fill struct with info needed.
            struct ParticleEmitter
            {
                std::vector<b2ParticleSystem*> ParticleSystem;
                std::vector<EntityID> ParticleEmitter;
                std::vector<EntityID> ParticleTemplate;
            };
            ParticleEmitter m_ParticleEmitters;

            std::list<Impulse> m_Impulses;



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

#endif

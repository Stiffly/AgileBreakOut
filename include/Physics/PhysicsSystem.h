#ifndef DAYDREAM_PHYSICSSYSTEM_H
#define DAYDREAM_PHYSICSSYSTEM_H

#include <unordered_map>
#include <random>

#include <Box2D/Box2D.h>

#include "Core/System.h"
#include "Core/World.h"
#include "Core/EventBroker.h"
#include "Core/CTemplate.h"
#include "Core/CTransform.h"
#include "Core/CTemplate.h"

#include "Physics/EContact.h"
#include "Physics/ESetImpulse.h"
#include "Physics/CRectangleShape.h"
#include "Physics/CCircleShape.h"
#include "Physics/CPhysics.h"
#include "Physics/CWaterVolume.h"
#include "Physics/CPhysics.h"
#include "Physics/CParticle.h"
#include "Physics/ECreateParticleSequence.h"
#include "Physics/CCircleShape.h"
#include "Physics/CParticleEmitter.h"

#include "Transform/TransformSystem.h"
#include "Transform/TransformSystem.h"

#include "CRectangleShape.h"

#include "Game/EStageCleared.h"
#include "Game/EPause.h"
#include "Game/CPad.h"
#include "Game/CBrick.h"
#include "Game/CBall.h"

#include "Rendering/CSprite.h"
#include "Rendering/CModel.h"


namespace dd
{

    namespace Systems
    {

        class PhysicsSystem : public System
        {
            friend class ContractListener;
            friend class DestructionListener;

        public:
            PhysicsSystem(World* world, std::shared_ptr<dd::EventBroker> eventBroker)
                    : System(world, eventBroker) { }

            ~PhysicsSystem();

	EventRelay<PhysicsSystem, Events::CreateParticleSequence> m_ECreateParticleSequence;
    	bool CreateParticleSequence(const Events::CreateParticleSequence &event);

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
                b2Vec2 Vector;
                b2Vec2 Point;
            };
            bool m_Pause = false;
            
            std::mt19937 gen;


            b2Vec2 m_Gravity = b2Vec2(0.f, -9.82f);
            b2World* m_PhysicsWorld = nullptr;
            float m_TimeStep = 1.f/60.f;
            float m_Accumulator = 0.f;
            int m_VelocityIterations = 6;
            int m_PositionIterations = 2;

            b2ParticleSystem* m_WaterParticleSystem;
            std::vector<b2ParticleGroup*> t_ParticleGroup;

            std::unordered_map<const b2ParticleHandle*, EntityID> m_WaterParticleHandleToEntities;
            std::unordered_map<EntityID, const b2ParticleHandle*> m_EntitiesToWaterParticleHandle;
            std::vector<std::unordered_map<EntityID, const b2ParticleHandle*>> m_EntitiesToParticleHandle;
            std::vector<std::unordered_map<const b2ParticleHandle*, EntityID>> m_ParticleHandleToEntities;
            std::unordered_map<EntityID, b2Body*> m_EntitiesToBodies;
            std::unordered_map<b2Body*, EntityID> m_BodiesToEntities;



            void CreateBody(EntityID entity);
            void SyncEntitiesWithBodies();
            void SyncBodiesWithEntities();
            void InitializeWater();
            void CreateParticleGroup(EntityID entity);

             b2ParticleSystem* CreateParticleSystem(float radius, float gravityScale, int maxCount);
            void CreateParticleEmitter(EntityID entity);
            void UpdateParticleEmitters(double dt); //TODO: Remove them and particles if needed.

            EventRelay<PhysicsSystem, Events::SetImpulse> m_SetImpulse;
            bool SetImpulse(const Events::SetImpulse &event);
            dd::EventRelay<PhysicsSystem, dd::Events::Pause> m_EPause;
            bool OnPause(const dd::Events::Pause &event);
			dd::EventRelay<PhysicsSystem, dd::Events::StageCleared> m_EStageCleared;
			bool OnStageCleared(const dd::Events::StageCleared &event);
			dd::EventRelay<PhysicsSystem, dd::Events::Contact> m_EContact;
			bool OnContact(const dd::Events::Contact &event);
			bool m_Travelling = false;
			float m_Timer = 0;

            //TODO: Fill struct with info needed.
        struct EmitterHandler //TODO CHANGE NAMES
        {
            std::vector<b2ParticleSystem*> ParticleSystem;
            std::vector<EntityID> ParticleEmitter;
            std::vector<EntityID> ParticleTemplate;
        };
            EmitterHandler m_ParticleEmitters;

            std::list<Impulse> m_Impulses;
            
            
            class DestructionListener : public b2DestructionListener
    {
    public:
        DestructionListener(PhysicsSystem* physicsSystem)
                : m_PhysicsSystem(physicsSystem) { }

        void SayGoodbye(b2Joint*) {LOG_INFO("joint körs");};
        void SayGoodbye(b2Fixture*) {/*LOG_INFO("Fixture körs");*/};

        void SayGoodbye(b2ParticleSystem* particleSystem, int32 index) override
        {
            LOG_INFO("Particle ded");


            const b2ParticleHandle* handle = particleSystem->GetParticleHandleFromIndex(index);
            for (int i = 0; i < m_PhysicsSystem->m_ParticleEmitters.ParticleSystem.size(); i++) {
                if(m_PhysicsSystem->m_ParticleEmitters.ParticleSystem[i] == particleSystem) {
                    std::unordered_map<const b2ParticleHandle*, EntityID>::iterator it = m_PhysicsSystem->m_ParticleHandleToEntities[i].find(handle);
                    if(it != m_PhysicsSystem->m_ParticleHandleToEntities[i].end()) {
                        EntityID id = it->second;
                        m_PhysicsSystem->m_World->RemoveEntity(id);
                        m_PhysicsSystem->m_ParticleHandleToEntities[i].erase(it);
                        LOG_INFO("Removing from list");
                    }
                }
            }
        }
    private:
        PhysicsSystem* m_PhysicsSystem;
    };

     class ParticleContactDisabler : public b2ContactFilter
    {
    public:
        ParticleContactDisabler() { };

        //Particles to particles
        virtual bool ShouldCollide(b2ParticleSystem *particleSystem, int32 particleIndexA, int32 particleIndexB)
        {
            return false;
        }
        virtual bool ShouldCollide(b2Fixture* fixture, b2ParticleSystem* particleSystem, int32 particleIndex)
        {
            return false;
        }
		
    };





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

        DestructionListener* m_DestructionListener;
        ParticleContactDisabler* m_ParticleContactDisabler;
        ContactListener* m_ContactListener;
        };

    }

}

#endif

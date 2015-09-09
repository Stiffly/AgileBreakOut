#include "PrecompiledHeader.h"
#include "Core/Physics/PhysicsSystem.h"

void dd::Systems::PhysicsSystem::Initialize(){
    m_Gravity = b2Vec2(0.f, 9.82f);
    m_PhysicsWorld = new b2World(m_Gravity);

    m_TimeStep = 1.f/60.f;
    m_VelocityIterations = 6;
    m_PositionIterations = 2;
}

void dd::Systems::PhysicsSystem::Update(double dt){
    m_PhysicsWorld->Step(m_TimeStep, m_VelocityIterations, m_PositionIterations);
}

void dd::Systems::PhysicsSystem::UpdateEntity(double dt, EntityID entity, EntityID parent){

}


void dd::Systems::PhysicsSystem::OnEntityCommit(EntityID entity){

}


void dd::Systems::PhysicsSystem::OnEntityRemoved(EntityID entity){

}

dd::Systems::PhysicsSystem::~PhysicsSystem(){
    delete m_PhysicsWorld;
}


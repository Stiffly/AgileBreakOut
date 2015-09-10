#include "PrecompiledHeader.h"
#include "Core/Physics/PhysicsSystem.h"



void dd::Systems::PhysicsSystem::RegisterComponents(ComponentFactory* cf)
{

}

void dd::Systems::PhysicsSystem::Initialize()
{
    m_Gravity = b2Vec2(0.f, 9.82f);
    m_PhysicsWorld = new b2World(m_Gravity);

    m_TimeStep = 1.f/60.f;
    m_VelocityIterations = 6;
    m_PositionIterations = 2;
}

void dd::Systems::PhysicsSystem::Update(double dt)
{
    m_PhysicsWorld->Step(m_TimeStep, m_VelocityIterations, m_PositionIterations);

}

void dd::Systems::PhysicsSystem::UpdateEntity(double dt, EntityID entity, EntityID parent)
{

}


void dd::Systems::PhysicsSystem::OnEntityCommit(EntityID entity)
{

    auto physicsComponent = m_World->GetComponent<Components::Physics>(entity);
    if(physicsComponent)
        CreateBody(entity);

}


void dd::Systems::PhysicsSystem::OnEntityRemoved(EntityID entity)
{

}


void dd::Systems::PhysicsSystem::CreateBody(EntityID entity)
{
    auto physicsComponent = m_World->GetComponent<Components::Physics>(entity);
    if(!physicsComponent)
        return;

    auto transformComponent = m_World->GetComponent<Components::Transform>(entity);
    if(!transformComponent)
        return;

    b2BodyDef bodyDef;
    bodyDef.position.Set(transformComponent->Position.x, transformComponent->Position.y);
    bodyDef.angle = glm::eulerAngles(transformComponent->Orientation).z; //TODO: CHECK IF THIS IS CORRECT

    if(physicsComponent->Static)
        bodyDef.type = b2_staticBody;
    else
        bodyDef.type = b2_dynamicBody;

    b2Body* body = m_PhysicsWorld->CreateBody(&bodyDef);

    b2PolygonShape pShape;

    auto boxComponent = m_World->GetComponent<Components::BoxShape>(entity);
    if(boxComponent){
        pShape.SetAsBox(boxComponent->x, boxComponent->y);
        }

    if(physicsComponent->Static){
        body->CreateFixture(&pShape, 0); //Density kanske ska vara 0 på statiska kroppar
    }
    else{
        //TODO: FIX THIS SHIT INTO COMPONENTS
        b2FixtureDef fixtureDef;
        fixtureDef.shape = &pShape;
        fixtureDef.density = 1.f;
        fixtureDef.restitution = 1.f;
        fixtureDef.friction = 0.3f;
        body->CreateFixture(&fixtureDef);

    }

    m_Bodies.insert(std::make_pair(entity, body));
}




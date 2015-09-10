#include "PrecompiledHeader.h"
#include "Core/Physics/PhysicsSystem.h"



void dd::Systems::PhysicsSystem::RegisterComponents(ComponentFactory* cf)
{

}

void dd::Systems::PhysicsSystem::Initialize()
{
    m_Gravity = b2Vec2(0.f, -9.82f);
    m_PhysicsWorld = new b2World(m_Gravity);

    m_TimeStep = 1.f/60.f;
    m_VelocityIterations = 6;
    m_PositionIterations = 2;
}

void dd::Systems::PhysicsSystem::Update(double dt)
{
    m_PhysicsWorld->Step(m_TimeStep, m_VelocityIterations, m_PositionIterations);

    for(auto i : m_Bodies)
    {
        EntityID entity = i.first;
        b2Body* body = i.second;

        auto transformComponent = m_World->GetComponent<Components::Transform>(entity);


        if(m_World->GetEntityParent(entity) == 0) {
            b2Vec2 position = body->GetPosition();
            transformComponent->Position.x = position.x;
            transformComponent->Position.y = position.y;

            float angle = body->GetAngle();

            //TODO: CHECK IF THIS IS CORRECT
            transformComponent->Orientation =  glm::quat(glm::vec3(0, 0, -angle));
        }
       // auto absoluteTransform = m_World->GetSystem<Systems::TransformSystem>()->
    }
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
    if(!physicsComponent){
        LOG_ERROR("No PhysicsComponent in CreateBody");
        return;
    }


    auto transformComponent = m_World->GetComponent<Components::Transform>(entity);
    if(!transformComponent) {
        LOG_ERROR("No TransformComponent in CreateBody");
        return;
    }

    auto absoluteTransform = m_World->GetSystem<Systems::TransformSystem>()->AbsoluteTransform(entity);

    b2BodyDef bodyDef;
    bodyDef.position.Set(absoluteTransform.Position.x, absoluteTransform.Position.y);


    bodyDef.angle = glm::degrees(-glm::eulerAngles(absoluteTransform.Orientation).z); //TODO: CHECK IF THIS IS CORRECT

    if(physicsComponent->Static)
        bodyDef.type = b2_staticBody;
    else
        bodyDef.type = b2_dynamicBody;

    b2Body* body = m_PhysicsWorld->CreateBody(&bodyDef);

    b2PolygonShape pShape;

    auto boxComponent = m_World->GetComponent<Components::RectangleShape>(entity);
    if(boxComponent){

        pShape.SetAsBox(absoluteTransform.Scale.x/2, absoluteTransform.Scale.y/2);
        }

    if(physicsComponent->Static){
        body->CreateFixture(&pShape, 0); //Density kanske ska vara 0 på statiska kroppar
    }
    else{
        //TODO: FIX THIS SHIT INTO COMPONENTS
        b2FixtureDef fixtureDef;
        fixtureDef.shape = &pShape;
        fixtureDef.density = 1.f;
        fixtureDef.restitution = 0.2f;
        fixtureDef.friction = 0.3f;
        body->CreateFixture(&fixtureDef);

    }

    m_Bodies.insert(std::make_pair(entity, body));
}




#include "PrecompiledHeader.h"
#include "Physics/PhysicsSystem.h"



void dd::Systems::PhysicsSystem::RegisterComponents(ComponentFactory* cf)
{
    cf->Register<Components::CircleShape>();
}

void dd::Systems::PhysicsSystem::Initialize()
{
    m_ContactListener = new ContactListener(this);

    m_Gravity = b2Vec2(0.f, 0.f);
    m_PhysicsWorld = new b2World(m_Gravity);

    m_TimeStep = 1.f/60.f;
    m_VelocityIterations = 8;
    m_PositionIterations = 6;


    m_PhysicsWorld->SetContactListener(m_ContactListener);

    EVENT_SUBSCRIBE_MEMBER(m_SetImpulse, PhysicsSystem::SetImpulse);
}

bool dd::Systems::PhysicsSystem::SetImpulse(const Events::SetImpulse &event)
{
    b2Body* body = m_EntitiesToBodies[event.Entity];

    b2Vec2 impulse;
    impulse.x = event.Impulse.x;
    impulse.y = event.Impulse.y;

    b2Vec2 point;
    point.x = event.Point.x;
    point.y = event.Point.y;

    body->ApplyLinearImpulse(impulse, point, true);

    return true;
}

void dd::Systems::PhysicsSystem::Update(double dt)
{
    for (auto i : m_EntitiesToBodies) {
        EntityID entity = i.first;
        b2Body* body = i.second;

        auto transformComponent = m_World->GetComponent<Components::Transform>(entity);


        if (m_World->GetEntityParent(entity) == 0) {
            b2Vec2 position;
            position.x = transformComponent->Position.x;
            position.y = transformComponent->Position.y;

            float angle = -glm::eulerAngles(transformComponent->Orientation).z;

            body->SetTransform(position, angle);
        }
    }

    m_PhysicsWorld->Step(m_TimeStep, m_VelocityIterations, m_PositionIterations);

    for (auto i : m_EntitiesToBodies) {
        EntityID entity = i.first;
        b2Body* body = i.second;

        auto transformComponent = m_World->GetComponent<Components::Transform>(entity);


        if (m_World->GetEntityParent(entity) == 0) {
            b2Vec2 position = body->GetPosition();
            transformComponent->Position.x = position.x;
            transformComponent->Position.y = position.y;

            float angle = body->GetAngle();

            //TODO: CHECK IF THIS IS CORRECT
            transformComponent->Orientation =  glm::quat(glm::vec3(0, 0, -angle));
        }
    }
}

void dd::Systems::PhysicsSystem::UpdateEntity(double dt, EntityID entity, EntityID parent)
{

}


void dd::Systems::PhysicsSystem::OnEntityCommit(EntityID entity)
{
    auto physicsComponent = m_World->GetComponent<Components::Physics>(entity);
    if(physicsComponent) {
        CreateBody(entity);
    }
}


void dd::Systems::PhysicsSystem::OnEntityRemoved(EntityID entity)
{
    b2Body* body = m_EntitiesToBodies[entity];

    m_EntitiesToBodies.erase(entity);
    m_BodiesToEntities.erase(body);

    m_PhysicsWorld->DestroyBody(body);
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


    bodyDef.angle = -glm::eulerAngles(absoluteTransform.Orientation).z;

    if (physicsComponent->Static) {
        bodyDef.type = b2_staticBody;
    } else {
        bodyDef.type = b2_dynamicBody;
    }


    b2Body* body = m_PhysicsWorld->CreateBody(&bodyDef);

    b2Shape* pShape;

    auto boxComponent = m_World->GetComponent<Components::RectangleShape>(entity);
    if (boxComponent) {
        b2PolygonShape* bShape = new b2PolygonShape();
        bShape->SetAsBox(absoluteTransform.Scale.x/2, absoluteTransform.Scale.y/2); //TODO: THIS SUCKS DUDE 4?!?!?!?
        pShape = bShape;
    } else {
        auto circleComponent = m_World->GetComponent<Components::CircleShape>(entity);
        if (circleComponent) {
            pShape = new b2CircleShape();
            pShape->m_radius = absoluteTransform.Scale.x;


            if (absoluteTransform.Scale.x != absoluteTransform.Scale.y &&  absoluteTransform.Scale.y != absoluteTransform.Scale.z) {
                LOG_WARNING("Circles has to be of uniform scale.");
            }
            pShape->m_radius = absoluteTransform.Scale.x/2; //TODO: THIS ALSO SUCKS 4 WTH

        }
    }

    if(physicsComponent->Static) {
        body->CreateFixture(pShape, 0); //Density kanske ska vara 0 p� statiska kroppar
    }
    else {
        //TODO: FIX THIS SHIT INTO COMPONENTS
        b2FixtureDef fixtureDef;
        fixtureDef.shape = pShape;
        fixtureDef.density = 1.f;
        fixtureDef.restitution = 1.0f;
        fixtureDef.friction = 0.3f;
        body->CreateFixture(&fixtureDef);

    }

    delete pShape;

    m_EntitiesToBodies.insert(std::make_pair(entity, body));
    m_BodiesToEntities.insert(std::make_pair(body, entity));
}

dd::Systems::PhysicsSystem::~PhysicsSystem()
{
    if (m_ContactListener != nullptr) {
        delete m_ContactListener;
        m_ContactListener = nullptr;
    }
}
#include "PrecompiledHeader.h"
#include "Physics/PhysicsSystem.h"



void dd::Systems::PhysicsSystem::RegisterComponents(ComponentFactory* cf)
{
    cf->Register<Components::CircleShape>();
}

void dd::Systems::PhysicsSystem::Initialize()
{
    m_ContactListener = new ContactListener(this);

    m_Gravity = b2Vec2(0.f, -9.82f);
    m_PhysicsWorld = new b2World(m_Gravity);

    m_TimeStep = 1.f/60.f;
    m_VelocityIterations = 6;
    m_PositionIterations = 2;
    m_Accumulator = 0.f;

    m_PhysicsWorld->SetContactListener(m_ContactListener);

    InitializeWater();

    EVENT_SUBSCRIBE_MEMBER(m_SetImpulse, PhysicsSystem::SetImpulse);
}

void dd::Systems::PhysicsSystem::InitializeWater()
{
    b2ParticleSystemDef m_ParticleSystemDef;
    m_ParticleSystemDef.radius = 0.1f;

    m_ParticleSystem = m_PhysicsWorld->CreateParticleSystem(&m_ParticleSystemDef);
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
        if (! transformComponent)
            continue;

        if (body == nullptr) {
            //LOG_ERROR("This body should not exist");
            continue;
        }

        if (m_World->GetEntityParent(entity) == 0) {
            b2Vec2 position;
            position.x = transformComponent->Position.x;
            position.y = transformComponent->Position.y;

            float angle = -glm::eulerAngles(transformComponent->Orientation).z;

            body->SetTransform(position, angle);

        }
    }

    m_Accumulator += dt;
    while(m_Accumulator >= m_TimeStep)
    {
        m_PhysicsWorld->Step(m_TimeStep, m_VelocityIterations, m_PositionIterations);
        m_Accumulator -= dt;
    }

    for (auto i : m_EntitiesToBodies) {
        EntityID entity = i.first;
        b2Body* body = i.second;

        if (body == nullptr) {
            //LOG_ERROR("This body should not exist");
            continue;
        }

        auto transformComponent = m_World->GetComponent<Components::Transform>(entity);
        if (! transformComponent)
            continue;
        auto parent = m_World->GetEntityParent(entity);
        if (parent == 0) {
            b2Vec2 position = body->GetPosition();
            transformComponent->Position.x = position.x;
            transformComponent->Position.y = position.y;

            float angle = body->GetAngle();

            //TODO: CHECK IF THIS IS CORRECT
            transformComponent->Orientation =  glm::quat(glm::vec3(0, 0, -angle));

        }
    }

    b2Vec2* positionBuffer = m_ParticleSystem->GetPositionBuffer();
    for (auto i : m_EntitiesToParticleHandle) {
        EntityID entity = i.first;
        b2ParticleHandle* particleH = i.second;

        b2Vec2 positionB2 = positionBuffer[particleH->GetIndex()];
        glm::vec2 position = glm::vec2(positionB2.x, positionB2.y);


        EntityID entityParent = m_World->GetEntityParent(entity);
        auto transform = m_World->GetComponent<Components::Transform>(entity);
        auto transformParent = m_World->GetComponent<Components::Transform>(entityParent);

        transform->Position = glm::vec3(position.x, position.y, -10) - transformParent->Position;
    }
}

void dd::Systems::PhysicsSystem::UpdateEntity(double dt, EntityID entity, EntityID parent)
{

}

void dd::Systems::PhysicsSystem::OnEntityCommit(EntityID entity)
{
    auto physicsComponent = m_World->GetComponent<Components::Physics>(entity);
    auto waterComponent = m_World->GetComponent<Components::WaterVolume>(entity);

    if (physicsComponent && waterComponent) {
        LOG_ERROR("Entity has both water and physics component, this is illegal. The police has been alerted.");
        return;
    }
    if (physicsComponent) {
        CreateBody(entity);
    } else if (waterComponent) {
        CreateParticleGroup(entity);
    }
}


void dd::Systems::PhysicsSystem::OnEntityRemoved(EntityID entity)
{
    b2Body* body = m_EntitiesToBodies[entity];

    if (body != nullptr) {
        m_EntitiesToBodies.erase(entity);
        m_BodiesToEntities.erase(body);

        m_PhysicsWorld->DestroyBody(body);
    }

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
        bShape->SetAsBox(absoluteTransform.Scale.x/2, absoluteTransform.Scale.y/2);
        pShape = bShape;
    } else {
        auto circleComponent = m_World->GetComponent<Components::CircleShape>(entity);
        if (circleComponent) {
            pShape = new b2CircleShape();
            pShape->m_radius = absoluteTransform.Scale.x;


            if (absoluteTransform.Scale.x != absoluteTransform.Scale.y &&  absoluteTransform.Scale.y != absoluteTransform.Scale.z) {
                LOG_WARNING("Circles has to be of uniform scale.");
            }
            pShape->m_radius = absoluteTransform.Scale.x/2;

        }
    }

    if(physicsComponent->Static) {
        body->CreateFixture(pShape, 0); //Density kanske ska vara 0 på statiska kroppar
    }
    else {
        //TODO: FIX THIS SHIT INTO COMPONENTS
        b2FixtureDef fixtureDef;
        fixtureDef.shape = pShape;
        fixtureDef.density = 1.f;
        fixtureDef.restitution = 1.0f;
        fixtureDef.friction = 0.0f;
        body->CreateFixture(&fixtureDef);

    }

    delete pShape;

    m_EntitiesToBodies.insert(std::make_pair(entity, body));
    m_BodiesToEntities.insert(std::make_pair(body, entity));
}

void dd::Systems::PhysicsSystem::CreateParticleGroup(EntityID e)
{
    //TODO: Lägg alla pd i en lista
    auto transform = m_World->GetComponent<Components::Transform>(e);
    if (!transform) {
        LOG_ERROR("No Transform component in CreateParticleGroup");
        return;
    }

    b2ParticleGroupDef pd;
    b2PolygonShape shape;

    shape.SetAsBox(transform->Scale.x/2.f, transform->Scale.y/2.f);
    pd.shape = &shape;
    pd.flags = b2_tensileParticle;
    pd.linearVelocity = b2Vec2(0, 5.f);
    pd.position.Set(transform->Position.x, transform->Position.y);
    //TODO: PUT IN LIST
    t_watergroup = m_ParticleSystem->CreateParticleGroup(pd);
    b2Vec2* t_ParticlePositions = m_ParticleSystem->GetPositionBuffer();
    for(int i = 0; i < m_ParticleSystem->GetParticleCount(); i++){
        {
            auto t_waterparticle = m_World->CreateEntity(e);
            auto transformChild = m_World->AddComponent<Components::Transform>(t_waterparticle);
            //auto sprite = m_World->AddComponent<Components::Sprite>(t_waterparticle);



            transformChild->Position = glm::vec3(t_ParticlePositions[i].x - transform->Position.x, t_ParticlePositions[i].y - transform->Position.y, -9.5f);
            transformChild->Scale = glm::vec3(m_ParticleSystem->GetRadius())/transform->Scale;
            //sprite->SpriteFile = "Textures/Ball.png";
            m_World->CommitEntity(t_waterparticle);

            m_EntitiesToParticleHandle.insert(std::make_pair(t_waterparticle, m_ParticleSystem->GetParticleHandleFromIndex(i)));
            m_ParticleHandleToEntities.insert(std::make_pair( m_ParticleSystem->GetParticleHandleFromIndex(i), t_waterparticle));
        }

    }
    LOG_INFO("ParticleCount: %i", m_ParticleSystem->GetParticleCount());
}

dd::Systems::PhysicsSystem::~PhysicsSystem()
{
    if (m_ContactListener != nullptr) {
        delete m_ContactListener;
        m_ContactListener = nullptr;
    }
}

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

    m_TimeStep = 1.0/60.0;
    m_VelocityIterations = 6;
    m_PositionIterations = 2;
    m_Accumulator = 0.0;

    m_PhysicsWorld->SetContactListener(m_ContactListener);

    InitializeWater();

    EVENT_SUBSCRIBE_MEMBER(m_SetImpulse, &PhysicsSystem::SetImpulse);
}

void dd::Systems::PhysicsSystem::InitializeWater()
{
    float radius = 0.13f;
    float gravityScale = 1.0f;
    CreateParticleSystem(radius, gravityScale);
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


    Impulse i;
    i.Body = body;
    i.Impulse = impulse;
    i.Point = point;

    m_Impulses.push_back(i);

    return true;
}

void dd::Systems::PhysicsSystem::Update(double dt) {

    m_Accumulator += dt;
    while (m_Accumulator >= m_TimeStep) {

        for (auto i : m_EntitiesToBodies) {
            EntityID entity = i.first;
            b2Body *body = i.second;

            auto transformComponent = m_World->GetComponent<Components::Transform>(entity);
            if (!transformComponent) {
                continue;
                LOG_ERROR("RigidBody with no TransformComponent");
            }
            auto physicsComponent = m_World->GetComponent<Components::Physics>(entity);
            if (!physicsComponent) {
                continue;
                LOG_ERROR("RigidBody with no PhysicsComponent");
            }


            if (body == nullptr) {
                LOG_ERROR("This body should not exist");
                continue;
            }

            if (m_World->GetEntityParent(entity) == 0) { //TODO: Make this work with childs too
                b2Vec2 position;
                position.x = transformComponent->Position.x;
                position.y = transformComponent->Position.y;
                float angle = glm::eulerAngles(transformComponent->Orientation).z;
                body->SetTransform(position, angle);
                body->SetLinearVelocity(b2Vec2(transformComponent->Velocity.x, transformComponent->Velocity.y));
                body->SetGravityScale(physicsComponent->GravityScale);

                b2Filter filter;
                filter.categoryBits = physicsComponent->Category;
                filter.maskBits = physicsComponent->Mask;
                body->GetFixtureList()->SetFilterData(filter);
            }
        }

        UpdateParticleEmitters(dt);

        for (auto i : m_Impulses) {
            i.Body->ApplyLinearImpulse(i.Impulse, i.Point, true);
        }
        m_Impulses.clear();

        m_PhysicsWorld->Step(m_TimeStep, m_VelocityIterations, m_PositionIterations);


        for (auto i : m_EntitiesToBodies) {
            EntityID entity = i.first;
            b2Body *body = i.second;

            if (body == nullptr) {
                LOG_ERROR("This body should not exist");
                continue;
            }

            auto transformComponent = m_World->GetComponent<Components::Transform>(entity);
            if (!transformComponent)
                continue;
            auto parent = m_World->GetEntityParent(entity);
            if (parent == 0) {

                auto physicsComponent = m_World->GetComponent<Components::Physics>(entity);
                if (physicsComponent->Calculate) { //TODO: REPLACE THIS WITH PARTICLE COLLISION FILTERS
                    transformComponent->Position.x =
                            transformComponent->Position.x + (transformComponent->Velocity.x * m_TimeStep);
                    transformComponent->Position.y =
                            transformComponent->Position.y + (transformComponent->Velocity.y * m_TimeStep);
                }
                else {
                    b2Vec2 position = body->GetPosition();
                    transformComponent->Position.x = position.x;
                    transformComponent->Position.y = position.y;

                    float angle = body->GetAngle();
                    transformComponent->Orientation = glm::quat(glm::vec3(0, 0, angle));

                    b2Vec2 velocity = body->GetLinearVelocity();
                    transformComponent->Velocity.x = velocity.x;
                    transformComponent->Velocity.y = velocity.y;
                }
            }
        }

        for ( int e = 0; e < m_EntitiesToParticleHandle.size(); e++) {
            b2Vec2 *positionBuffer = m_ParticleSystem[e]->GetPositionBuffer();
            for (auto i : m_EntitiesToParticleHandle[e]){
                EntityID entity = i.first;
                b2ParticleHandle *particleH = i.second;

                b2Vec2 positionB2 = positionBuffer[particleH->GetIndex()];
                glm::vec2 position = glm::vec2(positionB2.x, positionB2.y);

                EntityID entityParent = m_World->GetEntityParent(entity);
                glm::vec3 parentTransform = glm::vec3(0.f);
                if(entityParent) {
                    auto tp = m_World->GetComponent<Components::Transform>(entityParent);
                    parentTransform = tp->Position;
                }
                auto transform = m_World->GetComponent<Components::Transform>(entity);

                transform->Position = glm::vec3(position.x, position.y, -10) - parentTransform;

            }
        }

        m_Accumulator -= dt;
    }
}


void dd::Systems::PhysicsSystem::UpdateEntity(double dt, EntityID entity, EntityID parent)
{

}

void dd::Systems::PhysicsSystem::OnEntityCommit(EntityID entity)
{
    auto physicsComponent = m_World->GetComponent<Components::Physics>(entity);
    auto waterComponent = m_World->GetComponent<Components::WaterVolume>(entity);
    auto particleEmitterComponent = m_World->GetComponent<Components::ParticleEmitter>(entity);

    if (physicsComponent && waterComponent) {
        LOG_ERROR("Entity has both water and physics component, this is illegal. The police has been alerted.");
        return;
    }
    if (physicsComponent && particleEmitterComponent) {
        LOG_ERROR("Entity has both particle and physics component, this is illegal. The police has been alerted.");
        return;
    }

    if (physicsComponent) {
        CreateBody(entity);
    } else if (waterComponent) {
        CreateParticleGroup(entity);
    } else if (particleEmitterComponent) {
        CreateParticleEmitter(entity);
    }
}


void dd::Systems::PhysicsSystem::OnEntityRemoved(EntityID entity)
{
    auto physics = m_World->GetComponent<Components::Physics>(entity);
    if (physics == nullptr) {
        return;
    }

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

    b2FixtureDef fixtureDef;
    fixtureDef.shape = pShape;
    fixtureDef.filter.categoryBits = physicsComponent->Category;
    fixtureDef.filter.maskBits = physicsComponent->Mask;


    if(physicsComponent->Static) {
        body->CreateFixture(&fixtureDef); //Density kanske ska vara 0 p� statiska kroppar
    }
    else {
        fixtureDef.shape = pShape;
        fixtureDef.density = 10.f;
        fixtureDef.restitution = 1.0f;
        fixtureDef.friction = 0.0f;
        body->CreateFixture(&fixtureDef);

    }

    delete pShape;


    body->SetGravityScale(physicsComponent->GravityScale);
    m_EntitiesToBodies.insert(std::make_pair(entity, body));
    m_BodiesToEntities.insert(std::make_pair(body, entity));
}

void dd::Systems::PhysicsSystem::CreateParticleGroup(EntityID e)
{
    auto transform = m_World->GetComponent<Components::Transform>(e);
    if (!transform) {
        LOG_ERROR("No Transform component in CreateParticleGroup");
    }
    b2ParticleGroupDef pd;
    b2PolygonShape shape;
    auto water = m_World->GetComponent<Components::WaterVolume>(e);
    if (water) {

        shape.SetAsBox(transform->Scale.x/2.f, transform->Scale.y/2.f);
        pd.shape = &shape;
        pd.flags = b2_tensileParticle;
        pd.position.Set(transform->Position.x, transform->Position.y);
        t_ParticleGroup.push_back(m_ParticleSystem[0]->CreateParticleGroup(pd));
        b2Vec2* t_ParticlePositions = m_ParticleSystem[0]->GetPositionBuffer();
        for(int i = 0; i < m_ParticleSystem[0]->GetParticleCount(); i++){
            {
                auto t_waterparticle = m_World->CreateEntity(e);
                auto transformChild = m_World->AddComponent<Components::Transform>(t_waterparticle);

                transformChild->Position = glm::vec3(t_ParticlePositions[i].x - transform->Position.x, t_ParticlePositions[i].y - transform->Position.y, -9.5f);
                transformChild->Scale = glm::vec3(m_ParticleSystem[0]->GetRadius())/transform->Scale;
                m_World->CommitEntity(t_waterparticle);

                m_EntitiesToParticleHandle[0].insert(std::make_pair(t_waterparticle, m_ParticleSystem[0]->GetParticleHandleFromIndex(i)));
                m_ParticleHandleToEntities[0].insert(std::make_pair( m_ParticleSystem[0]->GetParticleHandleFromIndex(i), t_waterparticle));
            }

        }

    }
}

void dd::Systems::PhysicsSystem::UpdateParticleEmitters(double dt)
{
    for (int i = 0; i < m_ParticleEmitters.ParticleSystem.size(); i++) {
        auto e = m_ParticleEmitters.ParticleEmitter[i];
        auto pt = m_ParticleEmitters.ParticleTemplate[i];
        auto ps = m_ParticleEmitters.ParticleSystem[i];


        auto emitter = m_World->GetComponent<Components::ParticleEmitter>(e);
        auto particleTemplate = m_World->GetComponent<Components::Particle>(pt);
        emitter->TimeSinceLastSpawn += dt;
        if(emitter->TimeSinceLastSpawn < emitter->SpawnRate) {
            continue;
        }
        emitter->TimeSinceLastSpawn -= emitter->SpawnRate;
        auto templateTransform = m_World->GetComponent<Components::Transform>(pt);

        b2ParticleDef particleDef;
        particleDef.flags = particleTemplate->Flags;
        //particleDef.color TODO: Implement this if we want color mixing and shit.
        particleDef.lifetime = particleTemplate->LifeTime;
        particleDef.velocity = b2Vec2(templateTransform->Velocity.x, templateTransform->Velocity.y);
        particleDef.position = b2Vec2(templateTransform->Position.x, templateTransform->Position.y);

        auto particle = m_World->CloneEntity(pt, 0);
        m_World->RemoveComponent<Components::Template>(particle);

        auto b2Particle = ps->CreateParticle(particleDef);
        auto b2ParticleHandle = ps->GetParticleHandleFromIndex(b2Particle);
        m_EntitiesToParticleHandle[i].insert(std::make_pair(particle, b2ParticleHandle));
        m_ParticleHandleToEntities[i].insert(std::make_pair(b2ParticleHandle, particle));

        //TODO: Ta bort detta
        int particles = 0;
        for (auto i : m_EntitiesToParticleHandle)
        {
            particles += i.size();
        }
        LOG_INFO("--I have %i particles.", particles);
    }
}

void dd::Systems::PhysicsSystem::CreateParticleEmitter(EntityID entity)
{
    auto childEntities = m_World->GetEntityChildren(entity);
    if (childEntities.empty()) {
        LOG_ERROR("Particle Emitter does not have any child. Please adopt one or this will not work.");
        return;
    }
    int pf = 0;
    dd::Components::Particle* particle;
    EntityID childEntity;
    for ( auto c : childEntities ) {
        auto p = m_World->GetComponent<Components::Particle>(c);
        auto particleTemplate = m_World->GetComponent<Components::Template>(c);
        if (!p) {
            continue;
            LOG_WARNING("--ParticleEmitter's Child is not a particle.");
        }
        if(!particleTemplate) {
            LOG_ERROR("ParticleEmitter's particleChild is not a template.");
            continue;
        }
        if (pf != 0) {
            LOG_WARNING("ParticleEmitter has more than one child that is a particleTemplate, only the first one is used.");
            break;
        }
        childEntity = c;
        particle = p;
        pf++;
    }
    //TODO: Skicka med fler flaggor till particlesystemet;
    m_ParticleEmitters.ParticleSystem.push_back(CreateParticleSystem(particle->Radius, 0.f));
    m_ParticleEmitters.ParticleEmitter.push_back(entity);
    m_ParticleEmitters.ParticleTemplate.push_back(childEntity);
}


b2ParticleSystem* dd::Systems::PhysicsSystem::CreateParticleSystem(float radius, float gravityScale)
{
    std::unordered_map<EntityID, const b2ParticleHandle*> m1;
    std::unordered_map<const b2ParticleHandle*, EntityID> m2;
    m_EntitiesToParticleHandle.push_back(m1);
    m_ParticleHandleToEntities.push_back(m2);

    b2ParticleSystemDef m_ParticleSystemDef;
    m_ParticleSystemDef.radius = radius;
    m_ParticleSystemDef.gravityScale = gravityScale;

    m_ParticleSystem.push_back(m_PhysicsWorld->CreateParticleSystem(&m_ParticleSystemDef));

    return m_ParticleSystem.back();
}

dd::Systems::PhysicsSystem::~PhysicsSystem()
{
    if (m_ContactListener != nullptr) {
        delete m_ContactListener;
        m_ContactListener = nullptr;
    }
}

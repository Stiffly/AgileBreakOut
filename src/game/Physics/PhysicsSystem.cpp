#include "PrecompiledHeader.h"
#include "Physics/PhysicsSystem.h"


void dd::Systems::PhysicsSystem::RegisterComponents(ComponentFactory* cf)
{
    cf->Register<Components::CircleShape>();
    //TODO: REGISTER PHYSICS COMPONENTS HERE
}

void dd::Systems::PhysicsSystem::Initialize()
{
    std::random_device rd;
    gen = std::mt19937(rd());
//    m_DestructionListener = new DestructionListener(this);
    m_ContactListener = new ContactListener(this);
    m_PhysicsWorld = new b2World(m_Gravity);
	m_ParticleContactDisabler = new ParticleContactDisabler();
    m_PhysicsWorld->SetContactListener(m_ContactListener);
	m_PhysicsWorld->SetContactFilter(m_ParticleContactDisabler);
//	m_PhysicsWorld->SetDestructionListener(m_DestructionListener);

    InitializeWater();
    EVENT_SUBSCRIBE_MEMBER(m_SetImpulse, &PhysicsSystem::SetImpulse);
    EVENT_SUBSCRIBE_MEMBER(m_EPause, &PhysicsSystem::OnPause);
	EVENT_SUBSCRIBE_MEMBER(m_EResume, &PhysicsSystem::OnResume);
    EVENT_SUBSCRIBE_MEMBER(m_ECreateParticleSequence, &PhysicsSystem::CreateParticleSequence);
	EVENT_SUBSCRIBE_MEMBER(m_EContact, &PhysicsSystem::OnContact);
}

void dd::Systems::PhysicsSystem::InitializeWater()
{
	float radius = ResourceManager::Load<ConfigFile>("Config.ini")->GetValue<float>("Water.Radius", 0.2f);
    float gravityScale = 1.0f;

    b2ParticleSystemDef m_ParticleSystemDef;
    m_ParticleSystemDef.radius = radius;
    m_ParticleSystemDef.gravityScale = gravityScale;
	m_ParticleSystemDef.density = 4.f;

    m_WaterParticleSystem = m_PhysicsWorld->CreateParticleSystem(&m_ParticleSystemDef);
}

bool dd::Systems::PhysicsSystem::SetImpulse(const Events::SetImpulse &event)
{
    auto it = m_EntitiesToBodies.find(event.Entity);
    if(it == m_EntitiesToBodies.end()) {
        LOG_ERROR("Entity: %i, Tried to set an impulse on a body that does not exsist", event.Entity);
        return false;
    }

    b2Vec2 impulse;
    impulse.x = event.Impulse.x;
    impulse.y = event.Impulse.y;

    b2Vec2 point;
    point.x = event.Point.x;
    point.y = event.Point.y;

    Impulse i;
    i.Body = it->second;
    i.Vector = impulse;
    i.Point = point;

    m_Impulses.push_back(i);
    return true;
}

void dd::Systems::PhysicsSystem::SyncEntitiesWithBodies()
{

    for (auto i : m_EntitiesToBodies) {
        EntityID entity = i.first;
        b2Body* body = i.second;

        if (body == nullptr) {
            LOG_ERROR("This body should not exist, please fix this");
            continue;
        }

        auto transformComponent = m_World->GetComponent<Components::Transform>(entity);
        if (! transformComponent) {
            continue;
            LOG_ERROR("RigidBody with no TransformComponent");
        }

        auto physicsComponent = m_World->GetComponent<Components::Physics>(entity);
        if (! physicsComponent) {
            continue;
            LOG_ERROR("RigidBody with no PhysicsComponent");
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

			if (physicsComponent->CollisionType == CollisionType::Type::Static) {
				body->SetType(b2_staticBody);
			}
			else if (physicsComponent->CollisionType == CollisionType::Type::Dynamic) {
				body->SetType(b2_dynamicBody);
			}
			else if (physicsComponent->CollisionType == CollisionType::Type::Kinematic) {
				body->SetType(b2_dynamicBody);
			}
        }
    }
}

void dd::Systems::PhysicsSystem::SyncBodiesWithEntities()
{
    for (auto i : m_BodiesToEntities) {
        b2Body* body = i.first;
        EntityID entity = i.second;

        if (body == nullptr) {
            LOG_ERROR("This body should not exist, please fix this");
            continue;
        }

        auto transformComponent = m_World->GetComponent<Components::Transform>(entity);
        if (! transformComponent) {
            continue;
            LOG_ERROR("RigidBody with no TransformComponent");
        }

        auto physicsComponent = m_World->GetComponent<Components::Physics>(entity);
        if (! physicsComponent) {
            continue;
            LOG_ERROR("RigidBody with no PhysicsComponent");
        }

        auto parent = m_World->GetEntityParent(entity);
        if (parent == 0) {
            auto physicsComponent = m_World->GetComponent<Components::Physics>(entity);

            if (physicsComponent->Calculate) { //TODO: REPLACE THIS WITH PARTICLE COLLISION FILTERS
                transformComponent->Position.x = transformComponent->Position.x + (transformComponent->Velocity.x * m_TimeStep);
                transformComponent->Position.y = transformComponent->Position.y + (transformComponent->Velocity.y * m_TimeStep);
            } else {
                b2Vec2 position = body->GetPosition();
                transformComponent->Position.x = position.x;
                transformComponent->Position.y = position.y;

                float angle = body->GetAngle();
                transformComponent->Orientation =  glm::quat(glm::vec3(0, 0, angle));

                b2Vec2 velocity = body->GetLinearVelocity();
                transformComponent->Velocity.x = velocity.x;
                transformComponent->Velocity.y = velocity.y;
            }
        }
    }
}

void dd::Systems::PhysicsSystem::Update(double dt)
{
	if (m_Pause) {
		return;
	}
    m_Accumulator += dt;

    while(m_Accumulator >= m_TimeStep)
    {

        UpdateParticleEmitters(dt);
        SyncEntitiesWithBodies();

        //Apply Impulses Must be done after SyncEntitiesWithBodies
        for (auto i : m_Impulses) {
            i.Body->ApplyLinearImpulse(i.Vector, i.Point, true);
        }
        m_Impulses.clear();


        //Update the PhysicsWorld
        m_PhysicsWorld->Step(m_TimeStep, m_VelocityIterations, m_PositionIterations);

        SyncBodiesWithEntities();


        //water
        b2Vec2 *positionBuffer = m_WaterParticleSystem->GetPositionBuffer();
        for (auto i : m_EntitiesToWaterParticleHandle){
            EntityID entity = i.first;
            const b2ParticleHandle *particleH = i.second;


            b2Vec2 positionB2 = positionBuffer[particleH->GetIndex()];
            glm::vec2 position = glm::vec2(positionB2.x, positionB2.y);

            EntityID entityParent = m_World->GetEntityParent(entity);
            glm::vec3 parentTransform = glm::vec3(0.f);
            if(entityParent) {
                auto tp = m_World->GetComponent<Components::Transform>(entityParent);
                parentTransform = tp->Position;
            }
            auto transform = m_World->GetComponent<Components::Transform>(entity);

            transform->Position = glm::vec3(position.x - parentTransform.x, position.y - parentTransform.y, 0);
        }

        //Normal particles
        for ( int e = 0; e < m_EntitiesToParticleHandle.size(); e++) {
            b2Vec2 *positionBuffer = m_ParticleEmitters.ParticleSystem[e]->GetPositionBuffer();
            if(!positionBuffer) {
                break;
            }
            for (auto i : m_EntitiesToParticleHandle[e]){
                EntityID entity = i.first;
                const b2ParticleHandle *particleH = i.second;

                b2Vec2 positionB2 = positionBuffer[particleH->GetIndex()];
                glm::vec2 position = glm::vec2(positionB2.x, positionB2.y);

                EntityID entityParent = m_World->GetEntityParent(entity);
                glm::vec3 parentTransform = glm::vec3(0.f);
                if(entityParent) {
                    auto tp = m_World->GetComponent<Components::Transform>(entityParent);
                    parentTransform = tp->Position;
                }
                auto transform = m_World->GetComponent<Components::Transform>(entity);
				auto particle = m_World->GetComponent<Components::Particle>(entity);

                transform->Position = glm::vec3(position.x, position.y, transform->Position.z);
				transform->Scale = particle->Scale;//glm::vec3(particle->Radius * 2.f, particle->Radius * 2.f, 1);
            }
        }

        m_Accumulator -= m_TimeStep;
    }
}

void dd::Systems::PhysicsSystem::UpdateEntity(double dt, EntityID entity, EntityID parent)
{    
	if (m_Pause) {
		return;
	}

    auto particle = m_World->GetComponent<Components::Particle>(entity);
	auto pTemplate = m_World->GetComponent<Components::Template>(entity);

    if (particle && !pTemplate) {
        particle->TimeLived += dt;
		//Delete particles
		if (particle->TimeLived >= particle->LifeTime - 0.1) {
            for (int i = 0; i < m_EntitiesToParticleHandle.size(); i++) {
                const b2ParticleHandle* handle = m_EntitiesToParticleHandle[i][entity];
                std::unordered_map<EntityID, const b2ParticleHandle*>::iterator iter1;
                std::unordered_map<const b2ParticleHandle*,EntityID>::iterator iter2;
                iter1 = m_EntitiesToParticleHandle[i].find(entity);
                iter2 = m_ParticleHandleToEntities[i].find(handle);
                if (iter1 != m_EntitiesToParticleHandle[i].end()) {
                    m_EntitiesToParticleHandle[i].erase(iter1);
                }
                if (iter2 != m_ParticleHandleToEntities[i].end()) {
                    m_ParticleHandleToEntities[i].erase(iter2);
                }
                m_World->RemoveEntity(entity);
            }
        }
		//Update alpha
		auto sprite = m_World->GetComponent<Components::Sprite>(entity);
		float timeProgress = particle->TimeLived / (particle->LifeTime - 0.1);
		if (timeProgress > 1) {
			timeProgress = 1;
		}
		auto emitter = m_World->GetComponent<Components::ParticleEmitter>(particle->ParticleSystem);
		if (emitter) {
			sprite->Color.w = ScalarInterpolation(timeProgress, emitter->AlphaValues);
			particle->Scale = VectorInterpolation(timeProgress, emitter->ScaleValues);
			
		}
    }

	//Particlesystem relative to parent
	auto emitter = m_World->GetComponent<Components::ParticleEmitter>(entity);
	if (emitter) {
		EntityID emitterParent = emitter->Parent;
		if (emitterParent != 0) {
			auto emitterTransform = m_World->GetComponent<Components::Transform>(entity);
			auto parentTransform = m_World->GetComponent<Components::Transform>(emitterParent);
			if (emitterTransform && parentTransform) {
				emitterTransform->Position = parentTransform->Position;
			}
		}
	}
}

float dd::Systems::PhysicsSystem::ScalarInterpolation(float timeProgress, std::vector<float> spectrum)
{
	int spectrumSize = spectrum.size();
	if (spectrumSize == 0) {
		return 1.f;
	}
	if (spectrumSize == 1) {
		return spectrum[0];
	}

	float dValue = spectrum[1] - spectrum[0];
	float scalar = spectrum[0] + dValue * timeProgress;

	return scalar;
}

glm::vec3 dd::Systems::PhysicsSystem::VectorInterpolation(float timeProgress, std::vector<glm::vec3> spectrum)
{
	int spectrumSize = spectrum.size();
	if (spectrumSize == 0) {
		LOG_ERROR("You need to supply your particlesystem with scales in ScaleValues.");
		return glm::vec3(1);
	}
	else if (spectrumSize == 1) {
		return spectrum[0];
	}

	float dX = spectrum[1].x - spectrum[0].x;
	float x = spectrum[0].x + dX * timeProgress;
	float dY = spectrum[1].y - spectrum[0].y;
	float y = spectrum[0].y + dY * timeProgress;
	float dZ = spectrum[1].z - spectrum[0].z;
	float z = spectrum[0].z + dZ * timeProgress;

	return glm::vec3(x, y, z);
}

bool dd::Systems::PhysicsSystem::OnPause(const dd::Events::Pause &event)
{
    /*if (event.Type != "PhysicsSystem" && event.Type != "All") {
        return false;
    }*/
    m_Pause = true;
    return true;
}

bool dd::Systems::PhysicsSystem::OnResume(const dd::Events::Resume &event)
{
	/*if (event.Type != "PhysicsSystem" && event.Type != "All") {
	return false;
	}*/
	m_Pause = false;
	return true;
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
        auto it = m_EntitiesToBodies.find(entity);
        if(it == m_EntitiesToBodies.end()) {
            CreateBody(entity);
        } else {
            LOG_ERROR("Tried to commit a body that already exists");
        }
    } else if (waterComponent) {
        CreateParticleGroup(entity);
    } else if (particleEmitterComponent) {
        CreateParticleEmitter(entity);
    }
}


void dd::Systems::PhysicsSystem::OnEntityRemoved(EntityID entity)
{
    auto it = m_EntitiesToBodies.find(entity);
    if(it != m_EntitiesToBodies.end()) {
        it->second->GetWorld()->DestroyBody(it->second);
        m_BodiesToEntities.erase(it->second);
        m_EntitiesToBodies.erase(entity);
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

    if (physicsComponent->CollisionType == CollisionType::Type::Static) {
        bodyDef.type = b2_staticBody;
    } else if (physicsComponent->CollisionType == CollisionType::Type::Dynamic) {
        bodyDef.type = b2_dynamicBody;
    } else if (physicsComponent->CollisionType == CollisionType::Type::Kinematic) {
        bodyDef.type = b2_dynamicBody;
    }

    b2Body* body = m_PhysicsWorld->CreateBody(&bodyDef);

    b2Shape* pShape;

    auto rectangleComponent = m_World->GetComponent<Components::RectangleShape>(entity);
    if (rectangleComponent) {
        b2PolygonShape* bShape = new b2PolygonShape();
        bShape->SetAsBox(rectangleComponent->Dimensions.x/2.f, rectangleComponent->Dimensions.y/2.f);
        pShape = bShape;
    } else {
        auto circleComponent = m_World->GetComponent<Components::CircleShape>(entity);
        if (circleComponent) {
            pShape = new b2CircleShape();
            pShape->m_radius = circleComponent->Radius;
        }
    }

    b2FixtureDef fixtureDef;
    fixtureDef.shape = pShape;
    fixtureDef.filter.categoryBits = physicsComponent->Category;
    fixtureDef.filter.maskBits = physicsComponent->Mask;



    fixtureDef.shape = pShape;
    fixtureDef.density = physicsComponent->Density;
    fixtureDef.restitution = 1.0f;
    fixtureDef.friction = 0.0f;
    body->CreateFixture(&fixtureDef);


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
        t_ParticleGroup.push_back(m_WaterParticleSystem->CreateParticleGroup(pd));
        b2Vec2* t_ParticlePositions = m_WaterParticleSystem->GetPositionBuffer();
        for(int i = 0; i < m_WaterParticleSystem->GetParticleCount(); i++){
            {
                auto t_waterparticle = m_World->CreateEntity(e);
                auto transformChild = m_World->AddComponent<Components::Transform>(t_waterparticle);

                transformChild->Position = glm::vec3(t_ParticlePositions[i].x - transform->Position.x, t_ParticlePositions[i].y - transform->Position.y, -9.5f);
                transformChild->Scale = glm::vec3(m_WaterParticleSystem->GetRadius())/transform->Scale;
				transformChild->Scale *= 2;
                m_World->CommitEntity(t_waterparticle);

                m_EntitiesToWaterParticleHandle.insert(std::make_pair(t_waterparticle, m_WaterParticleSystem->GetParticleHandleFromIndex(i)));
                m_WaterParticleHandleToEntities.insert(std::make_pair(m_WaterParticleSystem->GetParticleHandleFromIndex(i), t_waterparticle));
            }

        }

    }
}


void dd::Systems::PhysicsSystem::UpdateParticleEmitters(double dt)
{
    std::vector<EntityID> emittersToDelete;
	int pSizeTest = 0; //For debugging, delete this
	int psListSize = 0;
    for (int i = 0; i < m_ParticleEmitters.ParticleSystem.size(); i++) {
        auto e = m_ParticleEmitters.ParticleEmitter[i];
        auto pt = m_ParticleEmitters.ParticleTemplate[i];
        auto ps = m_ParticleEmitters.ParticleSystem[i];
        pSizeTest += ps->GetParticleCount();
		psListSize++;
        auto emitter = m_World->GetComponent<Components::ParticleEmitter>(e);
        auto particleTemplate = m_World->GetComponent<Components::Particle>(pt);
        if(!emitter) {
            LOG_INFO("ParticleEmitter Component is nil");
            continue;
        }
        if(!particleTemplate) {
            LOG_INFO("ParticleTemplate Component is nil.");
            continue;
        }
        auto emitterTransform = m_World->GetComponent<Components::Transform>(e);
        emitterTransform->Orientation = glm::rotate(glm::quat(), 3.f, glm::vec3(0.f, 0.f, -1.f));

        emitter->TimeSinceLastSpawn += dt;
        emitter->LifeTime -= dt;

        if (emitter->LifeTime <= 0 || emitter->NumberOfTicks <= 0) {
            emittersToDelete.push_back(e);
            continue;
        }
        if(emitter->TimeSinceLastSpawn < emitter->SpawnRate || emitter->NumberOfTicks < 1) {
            continue;
        }
		
        emitter->NumberOfTicks--;
        emitter->TimeSinceLastSpawn -= emitter->SpawnRate;
        auto templateTransform = m_World->GetComponent<Components::Transform>(pt);

		//Create new particle
        for ( int j = 0; j < emitter->ParticlesPerTick; j++) {
			b2ParticleDef particleDef;
			//position
			particleDef.position = b2Vec2(emitterTransform->Position.x, emitterTransform->Position.y);
			if (particleDef.position.x == 0.f && particleDef.position.y == 0.f) {
				continue;
			}

			auto particle = m_World->CloneEntity(pt, 0);
			m_World->RemoveComponent<Components::Template>(particle);
			auto particleTransform = m_World->GetComponent<Components::Transform>(particle);
			auto particleComponent = m_World->GetComponent<Components::Particle>(particle);

            particleDef.flags = particleTemplate->Flags;
            //particleDef.color TODO: Implement this if we want color mixing and shit.
            particleDef.lifetime = particleTemplate->LifeTime;
            
			//random angle
			float eAngle = emitter->EmittingAngle;
            float halfSpread = emitter->Spread / 2;
            std::uniform_real_distribution<float> dis(eAngle - halfSpread, eAngle + halfSpread);
            float pAngle = dis(gen);

			//Using angle to determine velocity
			std::uniform_real_distribution<float> dis2(0.3, 1.5);
			float speedMultiplier = dis2(gen);
            glm::vec2 unitVec = glm::normalize(glm::vec2(glm::cos(pAngle), glm::sin(pAngle)));
			glm::vec2 vel = unitVec * emitter->Speed * (float)dt * speedMultiplier;
            particleDef.velocity = b2Vec2(vel.x, vel.y);

			//radius
			float radius = emitter->ScaleValues[0].x;
			float newRadius = 0;
			if (emitter->RadiusDistribution > 0) {
				float halfRadius = radius / 2;
				std::uniform_real_distribution<float> dis3(radius - halfRadius, radius + halfRadius);
				newRadius = dis3(gen);
			}
			else {
				newRadius = radius;
			}
			particleComponent->Scale = glm::vec3(newRadius);
	
			
			//Random z-value
            std::uniform_real_distribution<float> dist(0, 1);
            float zDistribution = dist(gen);
			particleTransform->Position = glm::vec3(emitterTransform->Position.x, emitterTransform->Position.y, -9.5f + zDistribution);
			particleTransform->Scale = particleTemplate->Scale;
				//glm::vec3(particleTemplate->Radius * 2.f / speedMultiplier, particleTemplate->Radius * 2.f / speedMultiplier, 1);
			particleTransform->Velocity = glm::vec3(vel.x, vel.y, 0);


            auto b2ParticleIndex = ps->CreateParticle(particleDef);
            auto b2ParticleHandle = ps->GetParticleHandleFromIndex(b2ParticleIndex);

            m_EntitiesToParticleHandle[i].insert(std::make_pair(particle, b2ParticleHandle));
            m_ParticleHandleToEntities[i].insert(std::make_pair(b2ParticleHandle, particle));
        }
		
    }
// 	system("cls");
// 	std::cout << "Particles living: " << pSizeTest << std::endl;
// 	std::cout << "particle systems: " << psListSize << std::endl;
// 	int entHandles = 0;
// 	for (int i = 0; i < m_EntitiesToParticleHandle.size(); i++)
// 		entHandles += m_EntitiesToParticleHandle[i].size();
// 	std::cout << "Entities and handles: " << entHandles << std::endl;
// 	LOG_INFO("Particles living: %i", pSizeTest);
// 	LOG_INFO("Particle systems: %i", psListSize);
// 	//int entHandles = 0;
// 	for (int i = 0; i < m_EntitiesToParticleHandle.size(); i++)
// 		entHandles += m_EntitiesToParticleHandle[i].size();
// 	LOG_INFO("Entities and handles: %i", entHandles);
	
	for (int i = 0; i < emittersToDelete.size(); i++)
    {
        EntityID ent = emittersToDelete[i];
        int key;
        for(key = 0; key < m_ParticleEmitters.ParticleEmitter.size(); key++) {
            if (m_ParticleEmitters.ParticleEmitter[key] == ent) {
                break;
            }
        }

        if (m_ParticleEmitters.ParticleSystem[key]->GetParticleCount() == 0 && key <= m_ParticleEmitters.ParticleSystem.size() - 1) {
			// DEBUG vvvvvv
// 			int t_Entities = m_World->GetEntities()->size();
// 			int t_Systems = m_ParticleEmitters.ParticleSystem.size();
// 			int t_Emitters = m_ParticleEmitters.ParticleEmitter.size();
// 			int t_Templates = m_ParticleEmitters.ParticleTemplate.size();
// 			int t_LFunParticles = 0;
// 			int t_EntParticles = 0;
// 			auto t_WorldEntities = m_World->GetEntities();
// 
// 			for (auto &te : *t_WorldEntities)
// 			{
// 				if (m_World->GetComponent<Components::Particle>(te.first))
// 				{
// 					t_EntParticles++;
// 				}
// 			}
// 
// 			for (int i = 0; i < t_Systems; i++)
// 			{
// 				t_LFunParticles = m_ParticleEmitters.ParticleSystem[i]->GetParticleCount();
// 			}
// 
// 			LOG_INFO("----------------------------");
// 			LOG_INFO("Deleting emitter and other");
// 			LOG_INFO("Total before:");
// 			LOG_INFO("Entities: %i, Systems: %i, Emitters: %i, Templates: %i, LfunParticles: %i, EntParticles: %i, EtH: %i, HtE: %i",
// 				t_Entities, t_Systems, t_Emitters, t_Templates, t_LFunParticles, t_EntParticles, m_EntitiesToParticleHandle.size(), m_ParticleHandleToEntities.size());
// 			
			// DEBUG ^^^^^

            m_World->RemoveEntity(ent);
            m_PhysicsWorld->DestroyParticleSystem(m_ParticleEmitters.ParticleSystem[key]);
            m_ParticleEmitters.ParticleSystem.erase(m_ParticleEmitters.ParticleSystem.begin() + key);
            m_ParticleEmitters.ParticleEmitter.erase(m_ParticleEmitters.ParticleEmitter.begin() + key);
            m_ParticleEmitters.ParticleTemplate.erase(m_ParticleEmitters.ParticleTemplate.begin() + key);
            m_EntitiesToParticleHandle.erase(m_EntitiesToParticleHandle.begin() + key);
            m_ParticleHandleToEntities.erase(m_ParticleHandleToEntities.begin() + key);
	    emittersToDelete.clear();
			// DEBUG vvvvv
// 			t_Entities = m_World->GetEntities()->size();
// 			t_Systems = m_ParticleEmitters.ParticleSystem.size();
// 			t_Emitters = m_ParticleEmitters.ParticleEmitter.size();
// 			t_Templates = m_ParticleEmitters.ParticleTemplate.size();
// 			t_LFunParticles = 0;
// 			t_EntParticles = 0;
// 			t_WorldEntities = m_World->GetEntities();
// 
// 			for (auto &te : *t_WorldEntities)
// 			{
// 				if (m_World->GetComponent<Components::Particle>(te.first))
// 				{
// 					t_EntParticles++;
// 				}
// 			}
// 
// 			for (int i = 0; i < t_Systems; i++)
// 			{
// 				t_LFunParticles = m_ParticleEmitters.ParticleSystem[i]->GetParticleCount();
// 			}
// 
// 			LOG_INFO("Total after:");
// 			LOG_INFO("Entities: %i, Systems: %i, Emitters: %i, Templates: %i, LfunParticles: %i, EntParticles: %i, EtH: %i, HtE: %i",
// 				t_Entities, t_Systems, t_Emitters, t_Templates, t_LFunParticles, t_EntParticles, m_EntitiesToParticleHandle.size(), m_ParticleHandleToEntities.size());
// 			LOG_INFO("-----------------------------");
// 			t_counter = 0;
			
			// DEBUG ^^^^^
        }
		
    }
}

bool dd::Systems::PhysicsSystem::CreateParticleSequence(const Events::CreateParticleSequence &event)
{
    //Creating Emitter
	EntityID emitter = m_World->CreateEntity();

    auto emitterTransform = m_World->AddComponent<Components::Transform>(emitter);
    auto particleEmitter= m_World->AddComponent<Components::ParticleEmitter>(emitter);

	if (event.parent != 0) {
		particleEmitter->Parent = event.parent;
	}

    emitterTransform->Position = event.Position;
    particleEmitter->GravityScale = event.GravityScale;
    particleEmitter->SpawnRate = event.SpawnRate;
    particleEmitter->NumberOfTicks = event.NumberOfTicks;
    particleEmitter->Speed = event.Speed;
    particleEmitter->ParticlesPerTick = event.ParticlesPerTick;
    particleEmitter->Spread = event.Spread;
    particleEmitter->EmittingAngle = event.EmittingAngle;
    particleEmitter->LifeTime = event.EmitterLifeTime;
	particleEmitter->AlphaValues = event.AlphaValues;
	particleEmitter->ScaleValues = event.ScaleValues;
	particleEmitter->RadiusDistribution = event.RadiusDistribution;
	{
		//Creating Particle Template
		auto particle = m_World->CreateEntity(emitter);
		auto particleTransform = m_World->AddComponent<Components::Transform>(particle);
		auto sprite = m_World->AddComponent<Components::Sprite>(particle);
		auto particleComponent = m_World->AddComponent<Components::Particle>(particle);
		m_World->AddComponent<Components::Template>(particle);
		particleTransform->Position = emitterTransform->Position;
		sprite->SpriteFile = event.SpriteFile;
		sprite->Color = event.Color;
		particleComponent->LifeTime = event.ParticleLifeTime;
		particleComponent->Flags = event.Flags;
		particleComponent->Scale = event.ScaleValues[0];
		particleComponent->ParticleSystem = emitter;
    }
    m_World->CommitEntity(emitter);
    return true;
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
            LOG_WARNING("ParticleEmitter's Child is not a particle.");
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

    m_ParticleEmitters.ParticleSystem.push_back(CreateParticleSystem(particle->Scale.x, 0.f, 0));
    m_ParticleEmitters.ParticleEmitter.push_back(entity);
    m_ParticleEmitters.ParticleTemplate.push_back(childEntity);
}


b2ParticleSystem* dd::Systems::PhysicsSystem::CreateParticleSystem(float radius, float gravityScale, int maxCount)
{
    std::unordered_map<EntityID, const b2ParticleHandle*> m1;
    std::unordered_map<const b2ParticleHandle*, EntityID> m2;
    m_EntitiesToParticleHandle.push_back(m1);
    m_ParticleHandleToEntities.push_back(m2);

    b2ParticleSystemDef m_ParticleSystemDef;
    m_ParticleSystemDef.maxCount = maxCount;
    m_ParticleSystemDef.radius = radius;
    m_ParticleSystemDef.gravityScale = gravityScale;
    m_ParticleSystemDef.destroyByAge = false;

    return m_PhysicsWorld->CreateParticleSystem(&m_ParticleSystemDef);
}

bool dd::Systems::PhysicsSystem::OnContact(const dd::Events::Contact &event)
{
	//Check if it is a brick colliding with ball
	auto brick = m_World->GetComponent<Components::Brick>(event.Entity1);
	EntityID entity = event.Entity1;
	Components::Model* model;
	auto ball = m_World->GetComponent<Components::Ball>(event.Entity2);
	if (!brick) {
		brick = m_World->GetComponent<Components::Brick>(event.Entity2);
		EntityID entity = event.Entity2;
		if (!brick) {
			return false;
		}
		else {
			model = m_World->GetComponent<Components::Model>(event.Entity2);
		}
	}
	else if (!ball) {
		ball = m_World->GetComponent<Components::Ball>(event.Entity1);
		if (!ball) {
			return false;
		}
	}
	else {

		model = m_World->GetComponent<Components::Model>(event.Entity1);
	}
	Events::PlaySound se;

	//Spawn a particle when a brick collides with something
	Events::CreateParticleSequence e;
	e.EmitterLifeTime = 3;
	e.EmittingAngle = glm::half_pi<float>();
	e.Spread = 0.f;
	e.NumberOfTicks = 1;
	e.ParticleLifeTime = 1.f;
	e.ParticlesPerTick = 1;
	e.Position = glm::vec3(event.IntersectionPoint.x, event.IntersectionPoint.y, -7);
	e.Color = glm::vec4(1.f);
	e.AlphaValues.push_back(1.f);
	e.AlphaValues.push_back(0.f);
	e.ScaleValues.push_back(glm::vec3(1.f));
	e.Speed = 0;

	auto PowerFriend = m_World->GetComponent<Components::MultiBallBrick>(entity);
	auto PowerSaviour = m_World->GetComponent<Components::LifebuoyBrick>(entity);
	auto PowerSticky = m_World->GetComponent<Components::StickyBrick>(entity);
	auto PowerInkBlaster = m_World->GetComponent<Components::InkBlasterBrick>(entity);
	auto PowerKraken = m_World->GetComponent<Components::KrakenAttackBrick>(entity);

	if (PowerFriend) {
		se.FilePath = "Sounds/jap-awesome.wav";
		EventBroker->Publish(se);
		e.SpriteFile = "Textures/PowerUps/Friends.png";
	} else if (PowerSaviour) {
		se.FilePath = "Sounds/jap-awesome.wav";
		EventBroker->Publish(se);
		e.SpriteFile = "Textures/PowerUps/Saviour.png";
	} else if (PowerSticky) {
		se.FilePath = "Sounds/jap-awesome.wav";
		EventBroker->Publish(se);
		e.SpriteFile = "Textures/PowerUps/Sticky.png";
	} else if (PowerInkBlaster){
		se.FilePath = "Sounds/jap-awesome.wav";
		EventBroker->Publish(se);
		e.SpriteFile = "Textures/PowerUps/InkBlaster.png";
	} else if (PowerKraken) { 
		se.FilePath = "Sounds/jap-huh.wav";
		EventBroker->Publish(se);
		e.SpriteFile = "Textures/PowerUps/RealeaseTheKraken.png";
	}
	else {
		e.EmitterLifeTime = 4;
		e.EmittingAngle = glm::half_pi<float>();
		e.Spread = 0.5f;
		e.NumberOfTicks = 1;
		e.ParticleLifeTime = 0.5f;
		e.ParticlesPerTick = 1;
		e.Position = glm::vec3(event.IntersectionPoint.x, event.IntersectionPoint.y, -10);
		e.ScaleValues.clear();
		e.ScaleValues.push_back(glm::vec3(0.5f));
		e.ScaleValues.push_back(glm::vec3(4.f, 4.f, 0.2f));
		e.SpriteFile = "Textures/Particles/Cloud_Particle.png";
		e.Color = model->Color;
		e.AlphaValues.clear();
		e.AlphaValues.push_back(1.f);
		e.AlphaValues.push_back(0.f);
		//e.Speed = 0;
	}

	EventBroker->Publish(e);
	return true;

	
}


dd::Systems::PhysicsSystem::~PhysicsSystem()
{
    for (auto i = m_BodiesToEntities.begin(); i != m_BodiesToEntities.end(); i++) {
        b2Body* body = i->first;
        body->GetWorld()->DestroyBody(body);
        //delete body;
    }
    m_BodiesToEntities.clear();
    m_EntitiesToBodies.clear();
    m_Impulses.clear();

    //TODO:REMOVE PARTICLE SYSTEMS

    if (m_ContactListener != nullptr) {
        delete m_ContactListener;
        m_ContactListener = nullptr;
    }

    if (m_PhysicsWorld != nullptr) {
        delete m_PhysicsWorld;
        m_PhysicsWorld = nullptr;
    }
}

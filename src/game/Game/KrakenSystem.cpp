//
// Created by Adniklastrator on 2015-10-15.
//

#include "PrecompiledHeader.h"
#include "Game/KrakenSystem.h"

void dd::Systems::KrakenSystem::Initialize()
{
	std::random_device rd;
	m_RandomGenerator = std::mt19937(rd());
    EVENT_SUBSCRIBE_MEMBER(m_EPause, &KrakenSystem::OnPause);
	EVENT_SUBSCRIBE_MEMBER(m_EResume, &KrakenSystem::OnResume);
	EVENT_SUBSCRIBE_MEMBER(m_EContact, &KrakenSystem::OnContact);
	EVENT_SUBSCRIBE_MEMBER(m_EKrakenAppear, &KrakenSystem::OnKrakenAppear);
	EVENT_SUBSCRIBE_MEMBER(m_EKrakenAttack, &KrakenSystem::OnKrakenAttack);
	EVENT_SUBSCRIBE_MEMBER(m_EKrakenHit, &KrakenSystem::OnKrakenHit);
	EVENT_SUBSCRIBE_MEMBER(m_EKrakenDefeated, &KrakenSystem::OnKrakenDefeated);
	EVENT_SUBSCRIBE_MEMBER(m_EBrickGenerating, &KrakenSystem::OnBrickGenerating);

	EntityID ent = m_World->CreateEntity();
	auto transform = m_World->AddComponent<Components::Transform>(ent);
	auto model = m_World->AddComponent<Components::Model>(ent);
	auto kraken = m_World->AddComponent<Components::Kraken>(ent);
	std::shared_ptr<Components::RectangleShape> cCircle = m_World->AddComponent<Components::RectangleShape>(ent);
	cCircle->Dimensions = glm::vec2(2, 4);
	//cCircle->Radius = 2;
	std::shared_ptr<Components::Physics> cPhys = m_World->AddComponent<Components::Physics>(ent);
	std::shared_ptr<Components::Template> cTemplate = m_World->AddComponent<Components::Template>(ent);

	auto travels = m_World->AddComponent<Components::Travels>(ent);
	travels->CurrentlyTraveling = false;

	cPhys->CollisionType = CollisionType::Type::Static;
	cPhys->GravityScale = 0.f;
	cPhys->Category = CollisionLayer::Type::Brick;
	cPhys->Mask = static_cast<CollisionLayer::Type>(CollisionLayer::Type::Ball | CollisionLayer::Type::Projectile | CollisionLayer::Type::Wall | CollisionLayer::LifeBuoy);

	model->ModelFile = "Models/Test/Ball/Sid.obj";
	transform->Position = glm::vec3(50, 50, -10);
	//sound
	auto collisionSound = m_World->AddComponent<Components::CollisionSound>(ent);
	collisionSound->FilePath = "Sounds/Brick/LTTP_Boss_Hit.wav";
	m_World->CommitEntity(ent);

	m_KrakenTemplate = ent;

	InitializeObjects();
}

void dd::Systems::KrakenSystem::InitializeObjects()
{

}

void dd::Systems::KrakenSystem::Update(double dt)
{
	if (m_Pause) {
		return;
	}
}

void dd::Systems::KrakenSystem::UpdateEntity(double dt, EntityID entity, EntityID parent)
{
    if (IsPaused() || !m_KrakenBattle) {
        return;
    }

	auto templateCheck = m_World->GetComponent<Components::Template>(entity);
	if (templateCheck != nullptr){ return; }

	auto kraken = m_World->GetComponent<Components::Kraken>(entity);
	if (kraken != nullptr) {
		auto transform = m_World->GetComponent<Components::Transform>(entity);
		if (transform->Position.y < -10) {
			m_World->RemoveEntity(entity);
			return;
		}
		Events::KrakenAttack krakenAttack;
		switch (kraken->CurrentAction)
		{
		case 1: // Idle
			m_KrakenTimer += dt;
			if (m_KrakenTimer > m_KrakenSecondsToAction) {
				if (m_NumberOfActions == 0) {
					kraken->CurrentAction = 3;
					break;
				}
				m_KrakenTimer = 0;
				std::uniform_real_distribution<float> dist(1, 4.999f);
				float random = dist(m_RandomGenerator);
				//std::cout << random << std::endl;
				kraken->CurrentAction = random;
				//std::cout << kraken->CurrentAction << std::endl;
			}
			break;
		case 2: // Grabbing
			m_NumberOfActions++;
			kraken->CurrentAction = 5;
			krakenAttack.ChargeUpdate = 0;
			krakenAttack.KrakenStrength = 0.1;
			krakenAttack.PlayerStrength = 0.05;
			EventBroker->Publish(krakenAttack);
			break;
		case 3: // Brick Generating
			{
				m_NumberOfActions++;
				kraken->CurrentAction = 1;
				Events::BrickGenerating e;
				e.Origin1 = glm::vec3(-5, 7, -10);
				e.Origin2 = glm::vec3(5, 7, -10);
				std::uniform_real_distribution<float> dist(1, 5.999f);
				float random = dist(m_RandomGenerator);
				e.Set = random;
				EventBroker->Publish(e);
				break;
			}
		case 4: // Brick Generating second case
			{
				kraken->CurrentAction = 1;
				Events::BrickGenerating e;
				e.Origin1 = glm::vec3(-5, 7, -10);
				e.Origin2 = glm::vec3(5, 7, -10);
				std::uniform_real_distribution<float> dist(1, 5.999f);
				float random = dist(m_RandomGenerator);
				e.Set = random;
				EventBroker->Publish(e);
				break;
			}
		case 5: // Taunting
			if (m_ReturnToIdle) {
				m_ReturnToIdle = false;
				kraken->CurrentAction = Idle;
			}
			break;
		default:
			//LOG_INFO("Something's probably off about the random number generator.");
			break;
		}
	}
}

bool dd::Systems::KrakenSystem::OnPause(const dd::Events::Pause &event)
{
	/*if (event.Type != "KrakenSystem" && event.Type != "All") {
		return false;
	}*/

    m_Pause = true;

    return true;
}

bool dd::Systems::KrakenSystem::OnResume(const dd::Events::Resume &event)
{
	/*if (event.Type != "KrakenSystem" && event.Type != "All") {
	return false;
	}*/
	m_Pause = false;
	return true;
}

bool dd::Systems::KrakenSystem::OnContact(const dd::Events::Contact &event)
{
	Components::Kraken* kraken1 = m_World->GetComponent<Components::Kraken>(event.Entity1);
	Components::Kraken* kraken2 = m_World->GetComponent<Components::Kraken>(event.Entity2);
	Components::Kraken* kraken;

	if (kraken1 != nullptr && kraken2 != nullptr) {
		return false;
	}

	EntityID krakenEntity, otherEntitiy;

	//Which is the ball?
	if (kraken1 != nullptr) {
		krakenEntity = event.Entity1;
		otherEntitiy = event.Entity2;
		kraken = kraken1;
	}
	else if (kraken2 != nullptr) {
		krakenEntity = event.Entity2;
		otherEntitiy = event.Entity1;
		kraken = kraken2;
	}
	else {
		return false;
	}
	
	auto projectile = m_World->GetComponent<Components::Projectile>(otherEntitiy);
	if (projectile != nullptr) {
		Events::ScoreEvent es;
		es.Score = 23;
		Events::KrakenHit e;
		e.Kraken = krakenEntity;
		e.Hitter = otherEntitiy;
		e.MaxHealth = kraken->MaxHealth;
		e.CurrentHealth = kraken->Health;
		e.NewHealth = kraken->Health - 1;
		EventBroker->Publish(e);
		m_World->RemoveEntity(otherEntitiy);
		return true;
	}
	auto ball = m_World->GetComponent<Components::Ball>(otherEntitiy);
	if (ball != nullptr) {
		ball->Combo += 1;
		Events::ComboEvent ec;
		ec.Combo = ball->Combo;
		ec.Ball = otherEntitiy;
		EventBroker->Publish(ec);
		Events::ScoreEvent es;
		es.Score = ball->Combo * 23;
		Events::KrakenHit e;
		e.Kraken = krakenEntity;
		e.Hitter = otherEntitiy;
		e.MaxHealth = kraken->MaxHealth;
		e.CurrentHealth = kraken->Health;
		e.NewHealth = kraken->Health - 1;
		EventBroker->Publish(e);
	}
	return true;
}

bool dd::Systems::KrakenSystem::OnKrakenAppear(const dd::Events::KrakenAppear &event)
{
	auto ent = m_World->CloneEntity(m_KrakenTemplate);
	m_World->RemoveComponent<Components::Template>(ent);
	auto transform = m_World->GetComponent<Components::Transform>(ent);
	transform->Position = event.Position;
	m_KrakenBattle = true;
	return true;
}

bool dd::Systems::KrakenSystem::OnKrakenAttack(const dd::Events::KrakenAttack &event)
{
	if (event.ChargeUpdate >= 1) {
		m_ReturnToIdle = true;
	}
	return true;
}

bool dd::Systems::KrakenSystem::OnKrakenHit(const dd::Events::KrakenHit &event)
{
	auto kraken = m_World->GetComponent<Components::Kraken>(event.Kraken);
	kraken->Health--;
	//std::cout << kraken->Health << std::endl;
	//kraken->Health = -1;
	if (kraken->Health < 0) {
		Events::KrakenDefeated e;
		e.Kraken = event.Kraken;
		e.Hitter = event.Hitter;
		EventBroker->Publish(e);
	}
	return false;
}

bool dd::Systems::KrakenSystem::OnKrakenDefeated(const dd::Events::KrakenDefeated &event)
{
	m_KrakenBattle = false;

	auto kraken = m_World->GetComponent<Components::Kraken>(event.Kraken);
	auto physicsComponent = m_World->GetComponent<Components::Physics>(event.Kraken);
	/*physicsComponent->CollisionType = CollisionType::Type::Dynamic;
	physicsComponent->GravityScale = 1.f;*/
	physicsComponent->Mask = static_cast<CollisionLayer::Type>(CollisionLayer::Water | CollisionLayer::Wall);
	/*physicsComponent->Calculate = false;*/

	auto transformComponentBrick = m_World->GetComponent<Components::Transform>(event.Kraken);
	auto transformComponentHitter = m_World->GetComponent<Components::Transform>(event.Hitter);


	auto brickModel = m_World->GetComponent<Components::Model>(event.Kraken);

	auto cTransform = m_World->GetComponent<Components::Transform>(event.Kraken);
	/*cTransform->Position = cTransform->Position + transformComponentBrick->Position;
	glm::vec2 ballToBrick = 4.f * glm::normalize(glm::vec2(cTransform->Position.x - transformComponentHitter->Position.x, cTransform->Position.y - transformComponentHitter->Position.y));
	cTransform->Velocity = glm::vec3(ballToBrick.x, ballToBrick.y, 0.f);

	std::uniform_real_distribution<float> dist(-1.5f, 1.5f);
	int impulseStrength = dist(m_RandomGenerator);

	Events::SetImpulse i;
	i.Entity = event.Kraken;
	i.Impulse = glm::vec2(impulseStrength, 0.f);
	i.Point = glm::vec2(cTransform->Position.x, cTransform->Position.y + 0.5f);
	EventBroker->Publish(i);*/

	auto cModel = m_World->GetComponent<Components::Model>(event.Kraken);
	cModel->Color = brickModel->Color;
	cModel->Color *= 0.5f;

	/*m_World->RemoveComponent<Components::Template>(event.Kraken);
	m_World->SetEntityParent(event.Kraken, 0);
	m_World->CommitEntity(event.Kraken);*/

	//Particle trail
	Events::CreateParticleSequence trail;
	trail.parent = event.Kraken;
	trail.AlphaValues.push_back(1.f);
	trail.AlphaValues.push_back(0.f);
	trail.ScaleValues.push_back(glm::vec3(0.08f));
	trail.ScaleValues.push_back(glm::vec3(0.f));
	trail.RadiusDistribution = 1;
	trail.EmitterLifeTime = 2.f;
	trail.ParticleLifeTime = 1.f;
	trail.ParticlesPerTick = 1;
	trail.SpawnRate = 0.1f;
	trail.Speed = 10.f;
	trail.EmittingAngle = glm::half_pi<float>();
	trail.SpriteFile = "Textures/Particles/FadeBall.png";
	trail.Color = brickModel->Color;
	//p.Spread = ...
	EventBroker->Publish(trail);

	//ParticlePoof
	Events::CreateParticleSequence poof;
	poof.EmitterLifeTime = 4;
	poof.EmittingAngle = glm::half_pi<float>();
	poof.Spread = 0.5f;
	poof.NumberOfTicks = 1;
	poof.ParticleLifeTime = 1.5f;
	poof.ParticlesPerTick = 1;
	poof.Position = cTransform->Position;
	poof.ScaleValues.clear();
	poof.ScaleValues.push_back(glm::vec3(0.5f));
	poof.ScaleValues.push_back(glm::vec3(2.f, 2.f, 0.2f));
	poof.SpriteFile = "Textures/Particles/Cloud_Particle.png";
	poof.Color = brickModel->Color;
	poof.AlphaValues.clear();
	poof.AlphaValues.push_back(1.f);
	poof.AlphaValues.push_back(0.f);
	poof.Speed = 10;
	EventBroker->Publish(poof);


	return true;
}

bool dd::Systems::KrakenSystem::OnBrickGenerating(const dd::Events::BrickGenerating &event)
{
	return true;
}
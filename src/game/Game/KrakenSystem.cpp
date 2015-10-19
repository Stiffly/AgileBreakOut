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
	EVENT_SUBSCRIBE_MEMBER(m_EKrakenAppear, &KrakenSystem::OnKrakenAppear);
	EVENT_SUBSCRIBE_MEMBER(m_EKrakenAttack, &KrakenSystem::OnKrakenAttack);
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
    if (IsPaused()) {
        return;
    }

	auto templateCheck = m_World->GetComponent<Components::Template>(entity);
	if (templateCheck != nullptr){ return; }

	auto kraken = m_World->GetComponent<Components::Kraken>(entity);
	if (kraken != nullptr) {
		Events::KrakenAttack krakenAttack;
		switch (kraken->CurrentAction)
		{
		case 1: // Idle
			//m_KrakenTimer += dt;
			if (m_KrakenTimer > m_KrakenSecondsToAction) {
				m_KrakenTimer = 0;
				std::uniform_real_distribution<float> dist(1, 2.999f);
				float random = dist(m_RandomGenerator);
				std::cout << random << std::endl;
				kraken->CurrentAction = random;
				std::cout << kraken->CurrentAction << std::endl;
			}
			break;
		case 2: // Grabbing
			kraken->CurrentAction = 5;
			krakenAttack.ChargeUpdate = 0;
			krakenAttack.KrakenStrength = 0.1;
			krakenAttack.PlayerStrength = 0.05;
			EventBroker->Publish(krakenAttack);
			break;
		case 3: // Brick Generating
			break;
		case 4: // Moving
			break;
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

bool dd::Systems::KrakenSystem::OnBrickGenerating(const dd::Events::BrickGenerating &event)
{
	return true;
}
//
// Created by Adniklastrator on 2015-10-15.
//

#include "PrecompiledHeader.h"
#include "Game/KrakenSystem.h"

void dd::Systems::KrakenSystem::Initialize()
{
    EVENT_SUBSCRIBE_MEMBER(m_EPause, &KrakenSystem::OnPause);
	EVENT_SUBSCRIBE_MEMBER(m_EKrakenAppear, &KrakenSystem::OnKrakenAppear);
	EVENT_SUBSCRIBE_MEMBER(m_EKrakenAttack, &KrakenSystem::OnKrakenAttack);

	EntityID ent = m_World->CreateEntity();
	auto transform = m_World->AddComponent<Components::Transform>(ent);
	auto model = m_World->AddComponent<Components::Model>(ent);
	std::shared_ptr<Components::CircleShape> cCircle = m_World->AddComponent<Components::CircleShape>(ent);
	cCircle->Radius = 2;
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
	collisionSound->FilePath = "Sounds/Brick/shortbrickbreak.wav";
	m_World->CommitEntity(ent);

	m_KrakenTemplate = ent;

	InitializeObjects();
}

void dd::Systems::KrakenSystem::InitializeObjects()
{

}

void dd::Systems::KrakenSystem::UpdateEntity(double dt, EntityID entity, EntityID parent)
{
    if (IsPaused()) {
        return;
    }

}

bool dd::Systems::KrakenSystem::OnPause(const dd::Events::Pause &event)
{
    if (IsPaused()) {
        SetPause(false);
    } else {
        SetPause(true);
    }
    return true;
}

bool dd::Systems::KrakenSystem::OnKrakenAppear(const dd::Events::KrakenAppear &event)
{
	auto ent = m_World->CloneEntity(m_KrakenTemplate);
	m_World->RemoveComponent<Components::Template>(ent);
	auto transform = m_World->GetComponent<Components::Transform>(ent);
	transform->Position = event.Position;
	return true;
}

bool dd::Systems::KrakenSystem::OnKrakenAttack(const dd::Events::KrakenAttack &event)
{
	
	return true;
}

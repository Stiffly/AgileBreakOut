//
// Created by Adniklastrator on 2015-10-08.
//


#include "PrecompiledHeader.h"
#include "Game/ProjectileSystem.h"


void dd::Systems::ProjectileSystem::Initialize()
{
	EVENT_SUBSCRIBE_MEMBER(m_EContact, &ProjectileSystem::OnContact);
	EVENT_SUBSCRIBE_MEMBER(m_EPause, &ProjectileSystem::OnPause);
	EVENT_SUBSCRIBE_MEMBER(m_EResume, &ProjectileSystem::OnResume);
	EVENT_SUBSCRIBE_MEMBER(m_EInkBlaster, &ProjectileSystem::OnInkBlaster);
	EVENT_SUBSCRIBE_MEMBER(m_EKrakenAttack, &ProjectileSystem::OnKrakenAttack);
	EVENT_SUBSCRIBE_MEMBER(m_EKrakenAttackEnd, &ProjectileSystem::OnKrakenAttackEnd);
	EVENT_SUBSCRIBE_MEMBER(m_EHitPad, &ProjectileSystem::OnHitPad);
	EVENT_SUBSCRIBE_MEMBER(m_EActionButton, &ProjectileSystem::OnActionButton);

	//Ink Blast
	{
		auto ent = m_World->CreateEntity();
		m_World->SetProperty(ent, "Name", "Projectile");
		std::shared_ptr<Components::Transform> ctransform = m_World->AddComponent<Components::Transform>(ent);
		
		auto circleShape = m_World->AddComponent<Components::CircleShape>(ent);
		auto inkBlastTemplate = m_World->AddComponent<Components::Template>(ent);
		circleShape->Radius = 0.1f;
		auto physics = m_World->AddComponent<Components::Physics>(ent);
		physics->CollisionType = CollisionType::Type::Dynamic;
		physics->Category = CollisionLayer::Type::Projectile;
		physics->Mask = static_cast<CollisionLayer::Type>(CollisionLayer::Type::Pad | CollisionLayer::Type::Brick | CollisionLayer::Type::Wall);
		physics->Calculate = true;
		ctransform->Position = glm::vec3(0.f, 0.f, -10.f);
		ctransform->Scale = glm::vec3(0.6f, 0.6f, 0.6f);
// 		auto cModel = m_World->AddComponent<Components::Model>(ent);
// 		cModel->ModelFile = "Models/Test/Ball/Sid.obj";
		auto cSprite = m_World->AddComponent<Components::Sprite>(ent);
		cSprite->SpriteFile = "Textures/Particles/OilShot.png";
		auto projectile = m_World->AddComponent<Components::Projectile>(ent);

		m_World->CommitEntity(ent);

		m_InkBlastTemplate = ent;
	}

    return;
}

void dd::Systems::ProjectileSystem::Update(double dt)
{
	if (m_Pause) {
		return;
	}
}

void dd::Systems::ProjectileSystem::UpdateEntity(double dt, EntityID entity, EntityID parent)
{
    if (IsPaused()) {
        return;
    }
    auto templateCheck = m_World->GetComponent<Components::Template>(entity);
    if (templateCheck != nullptr){ return; }

	auto shot = m_World->GetComponent<Components::Projectile>(entity);
	if (shot != nullptr) {
		auto transform = m_World->GetComponent<Components::Transform>(entity);
		if (transform->Position.y > 8) {
			m_World->RemoveEntity(entity);
		}
	}
}

bool dd::Systems::ProjectileSystem::OnPause(const dd::Events::Pause &event)
{
    /*if (event.Type != "ProjectileSystem" && event.Type != "All") {
        return false;
    }*/
    
    m_Pause = true;

    return true;
}

bool dd::Systems::ProjectileSystem::OnResume(const dd::Events::Resume &event)
{
	/*if (event.Type != "ProjectileSystem" && event.Type != "All") {
	return false;
	}*/
	m_Pause = false;
	return true;
}

bool dd::Systems::ProjectileSystem::OnContact(const dd::Events::Contact &event)
{
    
    return true;
}

bool dd::Systems::ProjectileSystem::OnInkBlaster(const dd::Events::InkBlaster &event)
{
	m_InkBlaster = true;
	m_Shots = event.Shots;
	m_InkBlasterSpeed = event.Speed;
	return true;
}

bool dd::Systems::ProjectileSystem::OnHitPad(const dd::Events::HitPad &event)
{
	if (m_InkBlaster) {
		m_SquidLoaded = true;
		m_AttachedSquid = event.Ball;
	}
	return true;
}

bool dd::Systems::ProjectileSystem::OnKrakenAttack(const dd::Events::KrakenAttack &event)
{
	m_KrakenAttack = true;
	return true;
}

bool dd::Systems::ProjectileSystem::OnKrakenAttackEnd(const dd::Events::KrakenAttackEnd &event)
{
	m_KrakenAttack = false;
	return true;
}

bool dd::Systems::ProjectileSystem::OnActionButton(const dd::Events::ActionButton &event)
{
	if (!m_KrakenAttack) {
		if (m_InkBlaster && m_SquidLoaded) {
			auto ent = m_World->CloneEntity(m_InkBlastTemplate);
			m_World->RemoveComponent<Components::Template>(ent);
			auto projectile = m_World->GetComponent<Components::Projectile>(ent);
			projectile->Speed = m_InkBlasterSpeed;
			auto transform = m_World->GetComponent<Components::Transform>(ent);
			transform->Position = event.Position;
			transform->Velocity = glm::vec3(0, projectile->Speed, 0);
			m_Shots--;
			if (m_Shots <= 0) {
				auto ball = m_World->GetComponent<Components::Ball>(m_AttachedSquid);
				m_InkBlaster = false;
				m_SquidLoaded = false;
				ball->InkBlaster = false;
				ball->Sticky = false;
				ball->Waiting = true;

				{
					Events::InkBlasterOver e;
					e.Ball = m_AttachedSquid;
					EventBroker->Publish(e);
				}
			{
				Events::ActionButton e;
				EventBroker->Publish(e);
			}
			}
		}
	}
	return true;
}

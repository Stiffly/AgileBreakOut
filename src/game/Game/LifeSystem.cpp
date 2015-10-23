//
// Created by Adniklastrator on 2015-10-13.
//

#include "PrecompiledHeader.h"
#include "Game/LifeSystem.h"

void dd::Systems::LifeSystem::Initialize()
{
	EVENT_SUBSCRIBE_MEMBER(m_EPause, &LifeSystem::OnPause);
	EVENT_SUBSCRIBE_MEMBER(m_EResume, &LifeSystem::OnResume);
	EVENT_SUBSCRIBE_MEMBER(m_ELifeLost, &LifeSystem::OnLifeLost);

	for (int i = 0; i < Lives(); i++) {
		CreateLife(i);
	}

	m_GodMode = ResourceManager::Load<ConfigFile>("Config.ini")->GetValue<bool>("Cheat.GodMode", false);
}

void dd::Systems::LifeSystem::Update(double dt)
{
	if (m_Pause) {
		return;
	}
	if (Lives() < 0) {
		Events::KrakenAttackEnd ek;
		EventBroker->Publish(ek);

		Events::GameOver e;
		EventBroker->Publish(e);

		Events::Pause p;
		p.Type = "All";
		EventBroker->Publish(p);

		//TODO: Make this not so ugly
		SetLives(3);
	}
}

void dd::Systems::LifeSystem::UpdateEntity(double dt, EntityID entity, EntityID parent)
{
	if (m_Pause) {
		return;
	}

	auto templateCheck = m_World->GetComponent<Components::Template>(entity);
	if (templateCheck != nullptr){ return; }
	auto life = m_World->GetComponent<Components::Life>(entity);

	
	if (life != nullptr) {
		auto transform = m_World->GetComponent<Components::Transform>(entity);
		if (transform->Position.y < -8) {
			m_World->RemoveEntity(entity);
			return;
		}

		transform->Position.x += transform->Velocity.x * dt;
		transform->Position.y += transform->Velocity.y * dt;
		
		glm::vec2 dir = glm::normalize(glm::vec2(transform->Velocity.x, transform->Velocity.y));
		glm::vec2 up = glm::vec2(0.f, 1.f);
		float angle = glm::acos(glm::dot<float>(dir, up)) * glm::sign(dir.x);
		transform->Orientation = glm::rotate(glm::quat(), angle, glm::vec3(0.f, 0.f, -1.f));

		if (!life->Dead) {
			if (life->Left) {
				if (transform->Position.x < -3.8) {
					life->Left = false;
					transform->Velocity.x = m_LifeSpeed;
				}
			}
			else {
				if (transform->Position.x > 3.8) {
					life->Left = true;
					transform->Velocity.x = -m_LifeSpeed;
				}
			}

			float sin = life->SinusAltitude;
			life->SinusAltitude += life->SinusSpeed * dt;
			float y = glm::sin((sin)* glm::half_pi<float>() * life->Frequency) * life->Amplitude;
			transform->Velocity.y = y;

			if (Lives() != PastLives()) {
				if (life->Number + 1 == PastLives()) {
					life->Dead = true;
					auto model = m_World->GetComponent<Components::Model>(entity);
					model->Color *= 0.3;

					//Particle trail
					Events::CreateParticleSequence trail;
					trail.parent = entity;
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
					trail.Color = model->Color;
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
					poof.Position = transform->Position;
					poof.ScaleValues.clear();
					poof.ScaleValues.push_back(glm::vec3(0.5f));
					poof.ScaleValues.push_back(glm::vec3(2.f, 2.f, 0.2f));
					poof.SpriteFile = "Textures/Particles/Cloud_Particle.png";
					poof.Color = model->Color;
					poof.AlphaValues.clear();
					poof.AlphaValues.push_back(1.f);
					poof.AlphaValues.push_back(0.f);
					poof.Speed = 10;
					EventBroker->Publish(poof);

					//m_World->RemoveEntity(entity);
					SetPastLives(Lives());
				}
			}
		} else {
			transform->Velocity.y -= 0.5 * dt;
		}
	}
}

void dd::Systems::LifeSystem::CreateLife(int number)
{
	auto life = m_World->CreateEntity();
	std::shared_ptr<Components::Transform> transform = m_World->AddComponent<Components::Transform>(life);
	//transform->Position = glm::vec3(-1.5f + number * 0.15f, -2.f, -5.f);
	transform->Position = glm::vec3(-3.8f + number * 3.7f, -5.2f, -10.f);
	//transform->Scale = glm::vec3(0.1f, 0.1f, 0.1f);
	transform->Scale = glm::vec3(0.2f, 0.2f, 0.2f);
	transform->Velocity.x = m_LifeSpeed;

	std::shared_ptr<Components::Life> cLife = m_World->AddComponent<Components::Life>(life);
	cLife->Number = number;
	cLife->SinusAltitude = glm::half_pi<float>() * number;
	cLife->Amplitude = 0.2;
	cLife->Frequency = 1;
	cLife->SinusSpeed = 1;

	auto model = m_World->AddComponent<Components::Model>(life);
	model->ModelFile = "Models/Sid/Sid.dae";

	auto anim = m_World->AddComponent<Components::Animation>(life);
	anim->Speed = 0.5f;

	m_World->CommitEntity(life);
}

bool dd::Systems::LifeSystem::OnLifeLost(const dd::Events::LifeLost &event)
{
	if (!m_GodMode){
		SetLives(Lives() - 1);
		return true;
	}
	return true;
}

bool dd::Systems::LifeSystem::OnPause(const dd::Events::Pause &event)
{
	/*if (event.Type != "LifeSystem" && event.Type != "All") {
		return false;
	}*/
	m_Pause = true;
    return true;
}
bool dd::Systems::LifeSystem::OnResume(const dd::Events::Resume &event)
{
	/*if (event.Type != "LifeSystem" && event.Type != "All") {
	return false;
	}*/
	m_Pause = false;
	return true;
}
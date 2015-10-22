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

	if (Lives() != PastLives()) {
		auto life = m_World->GetComponent<Components::Life>(entity);
		if (life != nullptr) {
			if (life->Number + 1 == PastLives()) {
				m_World->RemoveEntity(entity);
				SetPastLives(Lives());
			}
		}
	}
}

void dd::Systems::LifeSystem::CreateLife(int number)
{
	auto life = m_World->CreateEntity();
	std::shared_ptr<Components::Transform> transform = m_World->AddComponent<Components::Transform>(life);
	transform->Position = glm::vec3(-1.5f + number * 0.15f, -2.f, -5.f);
	transform->Scale = glm::vec3(0.1f, 0.1f, 0.1f);

	std::shared_ptr<Components::Life> lifeNr = m_World->AddComponent<Components::Life>(life);
	lifeNr->Number = number;

	auto model = m_World->AddComponent<Components::Model>(life);
	model->ModelFile = "Models/Sid/Sid.dae";

	m_World->CommitEntity(life);
}

bool dd::Systems::LifeSystem::OnLifeLost(const dd::Events::LifeLost &event)
{
	if (!m_GodMode){
		SetLives(Lives() - 1);
		return true;
	}
	return false;
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
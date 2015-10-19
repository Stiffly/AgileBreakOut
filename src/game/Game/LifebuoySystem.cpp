//
// Created by Adniklastrator on 2015-10-08.
//


#include "PrecompiledHeader.h"
#include "Game/LifebuoySystem.h"


void dd::Systems::LifebuoySystem::Initialize()
{
    EVENT_SUBSCRIBE_MEMBER(m_EContact, &LifebuoySystem::OnContact);
	EVENT_SUBSCRIBE_MEMBER(m_EPause, &LifebuoySystem::OnPause);
	EVENT_SUBSCRIBE_MEMBER(m_EResume, &LifebuoySystem::OnResume);
	EVENT_SUBSCRIBE_MEMBER(m_ELifebuoy, &LifebuoySystem::OnLifebuoy);
	EVENT_SUBSCRIBE_MEMBER(m_ELifebuoyHit, &LifebuoySystem::OnLifebuoyHit);

	//Lifebuoy
	{
		auto ent = m_World->CreateEntity();
		m_World->SetProperty(ent, "Name", "Lifebuoy");
		std::shared_ptr<Components::Transform> ctransform = m_World->AddComponent<Components::Transform>(ent);
		ctransform->Position = glm::vec3(50.f, -4.8f, -10.f);
		auto rectangleShape = m_World->AddComponent<Components::RectangleShape>(ent);
		auto lifebuoyTemplate = m_World->AddComponent<Components::Template>(ent);
		rectangleShape->Dimensions = glm::vec2(0.9f, 0.19f);
		auto physics = m_World->AddComponent<Components::Physics>(ent);
		physics->CollisionType = CollisionType::Type::Static;
		physics->Category = CollisionLayer::LifeBuoy;
		physics->Mask = static_cast<CollisionLayer::Type>(CollisionLayer::Ball | CollisionLayer::Brick | CollisionLayer::LifeBuoy | CollisionLayer::Pad | CollisionLayer::Water);
		physics->Density = 1.0f;
		physics->GravityScale = 1;
		auto cModel = m_World->AddComponent<Components::Model>(ent);
		cModel->ModelFile = "Models/Lifebuoy/Lifebuoy1.obj";
		auto lifebuoy = m_World->AddComponent<Components::Lifebuoy>(ent);

		m_World->CommitEntity(ent);

		m_Template = ent;
	}

    return;
}

void dd::Systems::LifebuoySystem::Update(double dt)
{
	for (auto it = m_Lifebuoys.begin(); it != m_Lifebuoys.end();) {
		
		auto transformComponent = m_World->GetComponent<Components::Transform>(it->Entity);
		auto lifebuoyComponent = m_World->GetComponent<Components::Lifebuoy>(it->Entity);

		if (transformComponent->Position.y < m_DownEdge) {
			m_World->RemoveEntity(it->Entity);
			m_Lifebuoys.erase(it++);
		}
		else {
			
			if (transformComponent->Position.x > m_RightEdge) {
				transformComponent->Position = glm::vec3(m_LeftEdge + 0.1f, transformComponent->Position.y, transformComponent->Position.z);
			} else if (transformComponent->Position.x < m_LeftEdge) {
				transformComponent->Position = glm::vec3(m_RightEdge - 0.1f, transformComponent->Position.y, transformComponent->Position.z);
			}
			
			if (lifebuoyComponent->Hits <= 0) {
				auto physicsComponent = m_World->GetComponent<Components::Physics>(it->Entity);
				physicsComponent->Mask = CollisionLayer::Type::LifeBuoy;
				physicsComponent->GravityScale = 10.f;
				auto modelComponent = m_World->GetComponent<Components::Model>(it->Entity);
				modelComponent->Color = glm::vec4(0.5f, 0.5f, 0.5f, 0.f);
			}

			if (transformComponent->Position.y < m_DownLimit && lifebuoyComponent->Hits > 0) {
				transformComponent->Position.y = m_DownLimit;
			}

			++it;
		}
	}
}

void dd::Systems::LifebuoySystem::UpdateEntity(double dt, EntityID entity, EntityID parent)
{
    if (IsPaused()) {
        return;
    }
    auto templateCheck = m_World->GetComponent<Components::Template>(entity);
    if (templateCheck != nullptr){ return; }
}

bool dd::Systems::LifebuoySystem::OnPause(const dd::Events::Pause &event)
{
    /*if (event.Type != "LifebuoySystem" && event.Type != "All") {
        return false;
    }*/

    m_Pause = true;

    return true;
}

bool dd::Systems::LifebuoySystem::OnResume(const dd::Events::Resume &event)
{
	/*if (event.Type != "LifebuoySystem" && event.Type != "All") {
	return false;
	}*/
	m_Pause = false;
	return true;
}

bool dd::Systems::LifebuoySystem::OnContact(const dd::Events::Contact &event)
{
    
    return true;
}

bool dd::Systems::LifebuoySystem::OnLifebuoy(const dd::Events::Lifebuoy &event) 
{
	auto ent = m_World->CloneEntity(m_Template);
	m_World->SetProperty(ent, "Name", "Lifebuoy");
	m_World->RemoveComponent<Components::Template>(ent);
	auto transform = m_World->GetComponent<Components::Transform>(ent);
	auto physics = m_World->GetComponent<Components::Physics>(ent);
	physics->CollisionType = CollisionType::Type::Dynamic;
	transform->Position = glm::vec3(0.f, 0.f, -10.f);
	transform->Velocity = glm::vec3(20.f, 3.f, 0.f);
	LifebuoyInfo info;
	info.Entity = ent;
	m_Lifebuoys.push_back(info);
	
	return true;
}

bool dd::Systems::LifebuoySystem::OnLifebuoyHit(const dd::Events::LifebuoyHit &event)
{
	auto lifebuoyComponent = m_World->GetComponent<Components::Lifebuoy>(event.Lifebuoy);
	lifebuoyComponent->Hits -= 1;
	auto modelComponent = m_World->GetComponent<Components::Model>(event.Lifebuoy);
	modelComponent->ModelFile = "Models/Lifebuoy/Lifebuoy" + std::to_string(5 - lifebuoyComponent->Hits) + ".obj";
	return true;
}

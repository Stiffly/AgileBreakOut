//
// Created by Adniklastrator on 2015-10-08.
//


#include "PrecompiledHeader.h"
#include "Game/LifebuoySystem.h"


void dd::Systems::LifebuoySystem::Initialize()
{
    EVENT_SUBSCRIBE_MEMBER(m_EContact, &LifebuoySystem::OnContact);
	EVENT_SUBSCRIBE_MEMBER(m_EPause, &LifebuoySystem::OnPause);
	EVENT_SUBSCRIBE_MEMBER(m_ELifebuoy, &LifebuoySystem::OnLifebuoy);

	//Lifebuoy
	{
		auto ent = m_World->CreateEntity();
		m_World->SetProperty(ent, "Name", "Lifebuoy");
		std::shared_ptr<Components::Transform> ctransform = m_World->AddComponent<Components::Transform>(ent);
		ctransform->Position = glm::vec3(50.f, -4.8f, -10.f);
		auto rectangleShape = m_World->AddComponent<Components::RectangleShape>(ent);
		auto lifebuoyTemplate = m_World->AddComponent<Components::Template>(ent);
		rectangleShape->Dimensions = glm::vec2(1.f, 0.1f);
		auto physics = m_World->AddComponent<Components::Physics>(ent);
		physics->CollisionType = CollisionType::Type::Static;
		physics->Category = CollisionLayer::Type::Other;
		physics->Mask = static_cast<CollisionLayer::Type>(CollisionLayer::Ball | CollisionLayer::Pad | CollisionLayer::Wall | CollisionLayer::Water);
		physics->Density = 0.001;
		physics->GravityScale = 1;
		ctransform->Sticky = true;
		auto cModel = m_World->AddComponent<Components::Model>(ent);
		cModel->ModelFile = "Models/Ship/Ship.obj";
		auto lifebuoy = m_World->AddComponent<Components::Lifebuoy>(ent);

		m_World->CommitEntity(ent);

		m_Template = ent;
	}

    return;
}

void dd::Systems::LifebuoySystem::Update(double dt)
{

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
    if (event.Type != "LifebuoySystem" && event.Type != "All") {
        return false;
    }

    if (IsPaused()) {
        SetPause(false);
    } else {
        SetPause(true);
    }
    return true;
}


bool dd::Systems::LifebuoySystem::OnContact(const dd::Events::Contact &event)
{
    
    return true;
}

bool dd::Systems::LifebuoySystem::OnLifebuoy(const dd::Events::Lifebuoy &event) 
{
	auto ent = m_World->CloneEntity(m_Template);
	m_World->RemoveComponent<Components::Template>(ent);
	auto transform = m_World->GetComponent<Components::Transform>(ent);
	auto physics = m_World->GetComponent<Components::Physics>(ent);
	physics->CollisionType = CollisionType::Type::Dynamic;
	transform->Position = glm::vec3(event.Transform->Position.x, transform->Position.y, -10.f);
	return true;
}

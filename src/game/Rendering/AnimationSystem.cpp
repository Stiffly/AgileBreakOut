#include "PrecompiledHeader.h"
#include "Rendering/AnimationSystem.h"

void dd::Systems::AnimationSystem::RegisterComponents(ComponentFactory* cf)
{
	cf->Register<Components::Animation>();
}

void dd::Systems::AnimationSystem::Initialize()
{
	EVENT_SUBSCRIBE_MEMBER(m_EPause, &AnimationSystem::OnPause);
	EVENT_SUBSCRIBE_MEMBER(m_EResume, &AnimationSystem::OnResume);
}

void dd::Systems::AnimationSystem::Update(double dt)
{
	if (m_Paused) {
		return;
	}

	auto animations = m_World->GetComponentsOfType<Components::Animation>();
	if (animations == nullptr) {
		return;
	}

	for (auto& component : *animations) {
		auto animation = static_cast<Components::Animation*>(component.get());
		animation->Time += animation->Speed * dt;
	}
}

bool dd::Systems::AnimationSystem::OnPause(const Events::Pause& e)
{
	m_Paused = true;
	return true;
}

bool dd::Systems::AnimationSystem::OnResume(const Events::Resume& e)
{
	m_Paused = false;
	return true;
}

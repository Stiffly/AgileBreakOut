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
		EntityID ent = component->Entity;
		auto animationComponent = static_cast<Components::Animation*>(component.get());

		// Get animation information
		auto modelComponent = m_World->GetComponent<Components::Model>(ent);
		auto model = ResourceManager::Load<Model>(modelComponent->ModelFile);
		auto skeleton = model->m_Skeleton;
		auto animation = skeleton->GetAnimation(animationComponent->Name);

		if (animationComponent->Speed != 0.0) {
			double nextTime = animationComponent->Time + animationComponent->Speed * dt;
			if (glm::abs(nextTime) > animation->Duration) {
				if (!animationComponent->Loop) {
					animationComponent->Time = glm::sign(nextTime) * animation->Duration;
					animationComponent->Speed = 0.0;
					LOG_DEBUG("Animation \"%s\" on Entity %i finished.", animationComponent->Name.c_str(), ent);
					Events::AnimationComplete e;
					e.Entity = ent;
					e.Animation = animationComponent->Name;
				}
			}
			animationComponent->Time = nextTime;
		}
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

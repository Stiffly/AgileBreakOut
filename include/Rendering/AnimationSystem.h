#ifndef AnimationSystem_h__
#define AnimationSystem_h__

#include "Core/System.h"
#include "Core/World.h"
#include "Rendering/CAnimation.h"
#include "Game/EPause.h"
#include "Game/EResume.h"

namespace dd
{
namespace Systems
{

class AnimationSystem : public System
{
public:
	AnimationSystem(World* world, std::shared_ptr<dd::EventBroker> eventBroker)
		: System(world, eventBroker) { }

	void RegisterComponents(ComponentFactory* cf) override;
	void Initialize() override;
	void Update(double dt) override;
	
private:
	bool m_Paused = false;

	EventRelay<AnimationSystem, Events::Pause> m_EPause;
	EventRelay<AnimationSystem, Events::Resume> m_EResume;
	bool OnPause(const Events::Pause& e);
	bool OnResume(const Events::Resume& e);
};

}
}
#endif

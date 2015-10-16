#ifndef AnimationSystem_h__
#define AnimationSystem_h__

#include "Core/System.h"
#include "Core/World.h"
#include "Rendering/CAnimation.h"
#include "Game/EPause.h"

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
	bool OnPause(const Events::Pause& e);
};

}
}
#endif

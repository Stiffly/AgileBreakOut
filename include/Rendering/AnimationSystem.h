#ifndef AnimationSystem_h__
#define AnimationSystem_h__

#include "Core/System.h"
#include "Core/World.h"
#include "Rendering/CAnimation.h"

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
	void Update(double dt) override;
};

}
}
#endif

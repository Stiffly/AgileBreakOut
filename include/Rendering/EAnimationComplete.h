#ifndef RENDERING_EANIMATIONCOMPLETE_H__
#define RENDERING_EANIMATIONCOMPLETE_H__

#include "Core/Entity.h"
#include "Core/EventBroker.h"

namespace dd
{
namespace Events
{

struct AnimationComplete : Event
{
	EntityID Entity;
	std::string Animation;
};

}
}

#endif
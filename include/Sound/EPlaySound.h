#ifndef Events_PlaySFX_h__
#define Events_PlaySFX_h__

#include "Core/EventBroker.h"

namespace dd
{

namespace Events
{

struct PlaySound : Event
{
	std::string path;
	float volume = 1.f;
	float pitch = 1.f;
	bool isAmbient = false;
};

}

}

#endif
#ifndef EVENTS_PLAYSFX_H__
#define EVENTS_PLAYSFX_H__

#include "Core/EventBroker.h"

namespace dd
{
namespace Events
{

struct PlaySound : Event
{
	std::string FilePath;
	float Gain = 1.f;
	float Pitch = 1.f;
	bool IsAmbient = false;
};

}
}

#endif
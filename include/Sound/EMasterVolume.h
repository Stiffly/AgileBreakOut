#ifndef EVENTS_EMASTERVOLUME_H__
#define EVENTS_EMASTERVOLUME_H__

#include "Core/EventBroker.h"

namespace dd
{
namespace Events
{

struct MasterVolume : public Event
{
    float Gain = 1;
    //To determine what channel group to apply the change to.
    bool IsAmbient = false;
};

}
}

#endif

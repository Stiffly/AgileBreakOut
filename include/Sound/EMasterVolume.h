//
// Created by Adam on 2015-09-24.
//

#ifndef EVENTS_EMASTERVOLUME_H__
#define EVENTS_EMASTERVOLUME_H__

#include "Core/EventBroker.h"

namespace dd
{

namespace Events
{

struct MasterVolume : public Event
{
    float gain = 1;
    //To determine what channel group to apply the change to.
    bool isAmbient = false;
};

}

}

#endif

#ifndef EVENTS_EPLAYBGM_H__
#define EVENTS_EPLAYBGM_H__

#include "Core/EventBroker.h"

namespace dd
{

namespace Events
{

struct PlayBGM : public Event
{
    std::string path;
    float volume = 1;
    //TODO: Rethink this
    bool SupportBGM;
};

}

}

#endif


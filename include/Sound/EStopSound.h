#ifndef EVENTS_ESTOPSOUND_H__
#define EVENTS_ESTOPSOUND_H__

#include "Core/EventBroker.h"

namespace dd
{

namespace Events
{
struct StopSound : public Event
{
    std::string FilePath;
};

}
}

#endif

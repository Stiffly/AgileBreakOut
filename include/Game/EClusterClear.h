#ifndef DAYDREAM_ECLUSTERCLEAR_H
#define DAYDREAM_ECLUSTERCLEAR_H

#include "Core/EventBroker.h"
#include "Core/Entity.h"

namespace dd
{

namespace Events
{

struct ClusterClear : Event
{
	int ClusterCleared;
};

}

}

#endif //DAYDREAM_ECLUSTERCLEAR_H

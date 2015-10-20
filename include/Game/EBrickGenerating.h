//
// Created by Adniklastrator on 2015-10-16.
//

#ifndef DAYDREAM_EBRICKGENERATING_H
#define DAYDREAM_EBRICKGENERATING_H

#include "Core/EventBroker.h"

namespace dd
{

namespace Events
{

struct BrickGenerating : Event
{
	glm::vec3 Origin;
	int TargetLine;
	int Set;
};

}

}

#endif //DAYDREAM_EBRICKGENERATING_H

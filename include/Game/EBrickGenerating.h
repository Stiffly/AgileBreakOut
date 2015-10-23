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
	glm::vec3 Origin1;
	glm::vec3 Origin2;
	int TargetLine;
	int Set;
	int SetCluster;
	int Speed = 6;
};

}

}

#endif //DAYDREAM_EBRICKGENERATING_H

//
// Created by Adniklastrator on 2015-10-15.
//

#ifndef DAYDREAM_EKRAKENAPPEAR_H
#define DAYDREAM_EKRAKENAPPEAR_H

#include "Core/EventBroker.h"

namespace dd
{

namespace Events
{

struct KrakenAppear : Event
{
	int Health;
	glm::vec3 Position;
};

}

}

#endif //DAYDREAM_EKRAKENAPPEAR_H

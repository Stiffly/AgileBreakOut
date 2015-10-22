//
// Created by Adniklastrator on 2015-10-08.
//

#ifndef DAYDREAM_EINKBLASTER_H
#define DAYDREAM_EINKBLASTER_H

#include "Core/EventBroker.h"

namespace dd
{

namespace Events
{

struct InkBlaster : Event
{
    Components::Transform* Transform;
	int Shots;
	float Speed = 5;
};

}

}

#endif //DAYDREAM_EINKBLASTER_H

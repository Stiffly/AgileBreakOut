//
// Created by Adniklastrator on 2015-10-08.
//

#ifndef DAYDREAM_ESTICKYPAD_H
#define DAYDREAM_ESTICKYPAD_H

#include "Core/EventBroker.h"

namespace dd
{

namespace Events
{

struct StickyPad : Event
{
    Components::Transform* Transform;
	int Times;
};

}

}

#endif //DAYDREAM_ESTICKYPAD_H

//
// Created by Adniklastrator on 2015-10-08.
//

#ifndef DAYDREAM_EINKBLASTEROVER_H
#define DAYDREAM_EINKBLASTEROVER_H

#include "Core/EventBroker.h"

namespace dd
{

namespace Events
{

struct InkBlasterOver : Event
{
	EntityID Ball;
};

}

}

#endif //DAYDREAM_EINKBLASTEROVER_H

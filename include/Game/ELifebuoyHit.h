//
// Created by Adniklastrator on 2015-09-29.
//

#ifndef DAYDREAM_ELIFEBUOYHIT_H
#define DAYDREAM_ELIFEBUOYHIT_H

#include "Core/Entity.h"
#include "Core/EventBroker.h"

namespace dd
{

namespace Events
{

struct LifebuoyHit : Event
{
	EntityID Lifebuoy;
};

}

}

#endif //DAYDREAM_ELIFEBUOYHIT_H

//
// Created by Administrator on 2015-10-14.
//

#ifndef DAYDREAM_ERAISEWATER_H
#define DAYDREAM_ERAISEWATER_H

#include "Core/EventBroker.h"

namespace dd
{

namespace Events
{

struct RaiseWater : Event
{
	float Amount;
};

}

}

#endif //DAYDREAM_ERAISEWATER_H

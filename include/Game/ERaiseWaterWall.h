//
// Created by Administrator on 2015-10-20.
//

#ifndef DAYDREAM_ERAISEWATERWALL_H
#define DAYDREAM_ERAISEWATERWALL_H

#include "Core/EventBroker.h"

namespace dd
{

namespace Events
{

struct RaiseWaterWall : Event
{
	float Amount;
	float Speed;
};

}

}

#endif //DAYDREAM_ERAISEWATERWALL_H

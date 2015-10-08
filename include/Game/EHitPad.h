//
// Created by Administrator on 2015-10-05.
//

#ifndef DAYDREAM_EHITPAD_H
#define DAYDREAM_EHITPAD_H

#include "Core/EventBroker.h"
#include "Core/Entity.h"

namespace dd
{
namespace Events
{
struct HitPad : Event
{
	EntityID Pad;
	EntityID Ball;
};
}
}

#endif //DAYDREAM_EHITPAD_H

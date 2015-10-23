//
// Created by Administrator on 2015-10-13.
//

#ifndef DAYDREAM_ESCREENSHAKE_H
#define DAYDREAM_ESCREENSHAKE_H

#include "Core/EventBroker.h"

namespace dd
{

namespace Events
{

struct ScreenShake : Event
{
    float Time = 1.f;
	float Intensity = 1.f;
	float TimeTakenToCoolDown = 1.f;
};

}

}

#endif //DAYDREAM_ESCREENSHAKE_H

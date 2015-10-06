//
// Created by Administrator on 2015-09-30.
//

#ifndef DAYDREAM_EHITLAG_H
#define DAYDREAM_EHITLAG_H

#include "Core/EventBroker.h"

namespace dd
{

namespace Events
{

struct HitLag : Event
{
    float Time;
    std::string Type;
};

}

}

#endif //DAYDREAM_EHITLAG_H

//
// Created by Adniklastrator on 2015-10-08.
//

#ifndef DAYDREAM_ELIFEBUOY_H
#define DAYDREAM_ELIFEBUOY_H

#include "Core/EventBroker.h"

namespace dd
{

namespace Events
{

struct Lifebuoy : Event
{
    Components::Transform* Transform;
};

}

}

#endif //DAYDREAM_ELIFEBUOY_H

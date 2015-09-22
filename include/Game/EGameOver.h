//
// Created by Administrator on 2015-09-22.
//

#ifndef DAYDREAM_EGAMEOVER_H
#define DAYDREAM_EGAMEOVER_H

#include "Core/EventBroker.h"
#include "Core/Entity.h"

namespace dd
{

namespace Events
{

struct GameOver : Event
{
    EntityID Entity;

};

}

}

#endif //DAYDREAM_EGAMEOVER_H

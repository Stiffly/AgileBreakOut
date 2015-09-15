//
// Created by Adniklastrator on 2015-09-15.
//

#ifndef DAYDREAM_ELIFELOST_H
#define DAYDREAM_ELIFELOST_H

#include "Core/EventBroker.h"
#include "Core/Entity.h"

namespace dd
{

namespace Events
{

struct LifeLost : Event
{
    EntityID Entity;

};

}

}

#endif //DAYDREAM_ELIFELOST_H

//
// Created by Adniklastrator on 2015-09-29.
//

#ifndef DAYDREAM_ECOMBOEVENT_H
#define DAYDREAM_ECOMBOEVENT_H

#include "Core/Entity.h"
#include "Core/EventBroker.h"

namespace dd
{

namespace Events
{

struct ComboEvent : Event
{
    EntityID Ball;
    int Combo;
};

}

}

#endif //DAYDREAM_ECOMBOEVENT_H

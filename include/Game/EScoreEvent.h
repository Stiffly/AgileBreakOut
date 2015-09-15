//
// Created by Adniklastrator on 2015-09-15.
//

#ifndef DAYDREAM_ESCOREEVENT_H
#define DAYDREAM_ESCOREEVENT_H

#include "Core/EventBroker.h"

namespace dd
{

namespace Events
{

struct ScoreEvent : Event
{
    int score = 0;
};

}

}

#endif //DAYDREAM_ESCOREEVENT_H

//
// Created by Adniklastrator on 2015-09-23.
//

#ifndef DAYDREAM_EPOWERUPTAKEN_H
#define DAYDREAM_EPOWERUPTAKEN_H

#include "Core/EventBroker.h"

namespace dd
{

namespace Events
{

struct PowerUpTaken : Event
{
    std::string Name = "Name";
};

}

}

#endif //DAYDREAM_EPOWERUPTAKEN_H

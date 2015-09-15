//
// Created by Adniklastrator on 2015-09-14.
//

#ifndef DAYDREAM_EBALLFELLOFFSTAGE_H
#define DAYDREAM_EBALLFELLOFFSTAGE_H

#include "Core/EventBroker.h"
#include "Core/Entity.h"

namespace dd
{

namespace Events
{

struct BallFellOffStage : Event
{
    EntityID entity;

};

}

}

#endif //DAYDREAM_EBALLFELLOFFSTAGE_H

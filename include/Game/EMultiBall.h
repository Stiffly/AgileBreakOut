//
// Created by Adniklastrator on 2015-09-17.
//

#ifndef DAYDREAM_EMULTIBALL_H
#define DAYDREAM_EMULTIBALL_H

#include "Core/EventBroker.h"
#include "Core/Entity.h"

namespace dd
{

namespace Events
{

struct MultiBall : Event
{
    Components::Transform* padTransform;
};

}

}

#endif //DAYDREAM_EMULTIBALL_H

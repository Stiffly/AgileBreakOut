//
// Created by Adniklastrator on 2015-09-22.
//

#ifndef DAYDREAM_ECREATEPOWERUP_H
#define DAYDREAM_ECREATEPOWERUP_H

#include "Core/EventBroker.h"

namespace dd
{

namespace Events
{

struct CreatePowerUp : Event
{
    glm::vec3 Position;

};

}

}

#endif //DAYDREAM_ECREATEPOWERUP_H

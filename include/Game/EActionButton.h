//
// Created by Administrator on 2015-10-07.
//

#ifndef DAYDREAM_EACTIONBUTTON_H
#define DAYDREAM_EACTIONBUTTON_H

#include "Core/EventBroker.h"

namespace dd
{

namespace Events
{

struct ActionButton : Event
{
	glm::vec3 Position;
};

}

}

#endif //DAYDREAM_EACTIONBUTTON_H

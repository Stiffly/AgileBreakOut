//
// Created by Adniklastrator on 2015-10-19.
//

#ifndef DAYDREAM_ERESUME_H
#define DAYDREAM_ERESUME_H

#include "Core/EventBroker.h"

namespace dd
{

namespace Events
{

struct Resume : Event
{
	std::string Type;
};

}

}

#endif //DAYDREAM_ERESUME_H

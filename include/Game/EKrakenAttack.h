//
// Created by Adniklastrator on 2015-10-08.
//

#ifndef DAYDREAM_EKRAKENATTACK_H
#define DAYDREAM_EKRAKENATTACK_H

#include "Core/EventBroker.h"

namespace dd
{

namespace Events
{

struct KrakenAttack : Event
{
	double KrakenStrength;
	double PlayerStrength;
	double ChargeUpdate;
};

}

}

#endif //DAYDREAM_EKRAKENATTACK_H

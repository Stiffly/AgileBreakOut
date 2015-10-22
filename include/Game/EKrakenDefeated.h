//
// Created by Adniklastrator on 2015-10-19.
//

#ifndef DAYDREAM_EKRAKENDEFEATED_H
#define DAYDREAM_EKRAKENDEFEATED_H

#include "Core/EventBroker.h"

namespace dd
{

namespace Events
{

struct KrakenDefeated : Event
{
	EntityID Kraken;
	EntityID Hitter;
};

}

}

#endif //DAYDREAM_EKRAKENDEFEATED_H

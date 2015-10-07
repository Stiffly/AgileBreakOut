//
// Created by Adniklastrator on 2015-09-09.
//

#ifndef DAYDREAM_ESTAGECLEARED_H
#define DAYDREAM_ESTAGECLEARED_H

#include "Core/EventBroker.h"

namespace dd
{
namespace Events
{
struct StageCleared : Event
{
    int StageCleared = 0;
    int StageCluster = 0;
};
}
}

#endif //DAYDREAM_ESTAGECLEARED_H

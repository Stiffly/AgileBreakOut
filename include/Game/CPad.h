//
// Created by Adniklastrator on 2015-09-10.
//

#ifndef DAYDREAM_CPAD_H
#define DAYDREAM_CPAD_H

#include "Core/Component.h"
#include "Core/EKeyDown.h"
#include "Core/EventBroker.h"

namespace dd
{

namespace Components
{

struct Pad : Component
{
    int Lives;
    int Points;
};

}

}

#endif //DAYDREAM_CPAD_H

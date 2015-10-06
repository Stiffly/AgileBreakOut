//
// Created by Adniklastrator on 2015-09-09.
//

#ifndef DAYDREAM_CBRICK_H
#define DAYDREAM_CBRICK_H

#include "Core/Component.h"

namespace dd
{

namespace Components
{

struct Brick : Component
{
    int Score = 50;
    bool Removed = false;
};

}

}

#endif //DAYDREAM_CBRICK_H

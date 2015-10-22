//
// Created by Adniklastrator on 2015-09-15.
//

#ifndef DAYDREAM_CLIFE_H
#define DAYDREAM_CLIFE_H

#include "Core/Component.h"

namespace dd
{

namespace Components
{

struct Life : Component 
{
    int Number = 0;
	float SinusAltitude = 1;
	bool Left = false;
};

}

}

#endif //DAYDREAM_CLIFE_H

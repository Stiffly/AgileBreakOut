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
	float SinusAltitude = 0;
	float Amplitude = 1;
	float Frequency = 1;
	float SinusSpeed = 1;
	bool Left = false;
	bool Dead = false;
};

}

}

#endif //DAYDREAM_CLIFE_H

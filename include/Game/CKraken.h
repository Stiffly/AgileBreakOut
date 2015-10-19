//
// Created by Adniklastrator on 2015-10-15.
//

#ifndef DAYDREAM_CKRAKEN_H
#define DAYDREAM_CKRAKEN_H

#include "Core/Component.h"

namespace dd
{

namespace Components
{

struct Kraken : Component {

    int Health = 10;
	int CurrentAction = 1;
	
	const int Idle = 1;
	const int Grabbing = 2;
	const int Taunting = 3;
	const int Moving = 4;
	const int BrickGenerating = 5;
	// Idle. Generating bricks. Taunting. No idea.
};

}

}

#endif //DAYDREAM_CKRAKEN_H

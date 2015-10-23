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

    int MaxHealth = 30;
	int Health = MaxHealth;
	int CurrentAction = 3;
	float RetreatSpeed = 0.2;
	
	const int Idle = 1;
	const int Grabbing = 2;
	const int BrickGenerating = 3;
	const int BrickGenerating2 = 4;
	const int Waiting = 5;
	// Idle. Generating bricks. Taunting. No idea.
};

}

}

#endif //DAYDREAM_CKRAKEN_H

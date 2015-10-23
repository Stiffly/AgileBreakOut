//
// Created by Adniklastrator on 2015-09-10.
//

#ifndef DAYDREAM_CBALL_H
#define DAYDREAM_CBALL_H

#include "Core/Component.h"

namespace dd
{

namespace Components
{

struct Ball : Component
{
    int Number = 0;
    float Speed = 5.f;
    int Combo = 0;
    bool Paused = false;
    bool Waiting = true;
	bool InkBlaster = false;
	bool Sticky = false;
	bool Loaded = false;
	glm::vec3 StickyPlacement = glm::vec3(0, 0, 0);
    glm::vec3 SavedSpeed = glm::vec3(0, 1, 0);

	int PosterBoy = 0;
};

}

}

#endif //DAYDREAM_CBALL_H

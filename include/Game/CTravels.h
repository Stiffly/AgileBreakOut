//
// Created by Adniklastrator on 2015-10-15.
//

#ifndef DAYDREAM_CTRAVELS_H
#define DAYDREAM_CTRAVELS_H

#include "Core/Component.h"

namespace dd
{

namespace Components
{

struct Travels : Component 
{
	//This indicates that the object moves upon moving to the next stage. Place this on an object that is to move.
	//The camera does not move. Things that are to follow the camera should not have this component.
};

}

}

#endif //DAYDREAM_CTRAVELS_H

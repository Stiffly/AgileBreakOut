//
// Created by Adam on 2015-09-09.
//

#ifndef DAYDREAM_EPLAYSOUND_H
#define DAYDREAM_EPLAYSOUND_H

#include <string>
#include "EventBroker.h"


namespace dd
{

namespace Events
{

struct PlaySound
{
    std::string path;
};

}

}

#endif //DAYDREAM_EPLAYSOUND_H

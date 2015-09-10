#ifndef DAYDREAM_CPHYSICS_H
#define DAYDREAM_CPHYSICS_H
#include "Core/Component.h"

namespace dd
{
namespace Components
{

    struct Physics: public Component
    {
        //idk if anything should be here :)
        bool Static = true;
    };

}

}

#endif //DAYDREAM_CPHYSICS_H

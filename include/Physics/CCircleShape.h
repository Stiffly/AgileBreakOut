#ifndef CCIRCLESHAPE_H__
#define CCIRCLESHAPE_H__

#include "Core/Component.h"

namespace dd
{
namespace Components
{

struct CircleShape : public Component
{
    float Radius = 1.f;
};

}

}

#endif

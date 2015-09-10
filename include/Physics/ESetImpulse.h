#ifndef ESETIMPULSE_H__
#define ESETIMPULSE_H__

#include "Core/EventBroker.h"
#include "Core/Entity.h"

namespace dd
{
namespace Events
{

struct Contact : Event
{
    EntityID Entity;
    glm::vec2 Direction;
    float Strength;
};

}
}

#endif

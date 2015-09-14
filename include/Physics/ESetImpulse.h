#ifndef ESETIMPULSE_H__
#define ESETIMPULSE_H__

#include "Core/EventBroker.h"
#include "Core/Entity.h"

namespace dd
{
namespace Events
{

struct SetImpulse : Event
{
    EntityID Entity;
    glm::vec2 Impulse;
    glm::vec2 Point;
};

}
}

#endif

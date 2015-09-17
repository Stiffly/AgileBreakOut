//
// Created by Viktor on 10/09/2015.
//

#ifndef Events_ECONTACT_h__
#define Events_ECONTACT_h__


#include <glm/detail/type_vec.hpp>
#include "Core/EventBroker.h"
#include "Core/Entity.h"

namespace dd
{
namespace Events
{

struct Contact : Event
{
    EntityID Entity1;
    EntityID Entity2;
    glm::vec2 Normal;
};

}
}

#endif //Events_ECONTACT_h__

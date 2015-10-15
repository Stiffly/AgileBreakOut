#ifndef DAYDREAM_CRECTANGLESHAPE_H
#define DAYDREAM_CRECTANGLESHAPE_H

#include "Core/Component.h"

namespace dd
{
namespace Components
{

struct RectangleShape : public Component
{
    glm::vec2 Dimensions = glm::vec2(1.f, 1.f);
};

}

}

#endif //DAYDREAM_CRECTANGLESHAPE_H

#ifndef COMPONENTS_CCOLLISIONSOUND_H__
#define COMPONENTS_CCOLLISIONSOUND_H__

#include "Core/Component.h"

namespace dd
{
namespace Components
{

struct CollisionSound : public Component
{
    std::string FilePath;
};

}
}

#endif

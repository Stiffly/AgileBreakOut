#ifndef COMPONENTS_PARTICLEEMITTER_H__
#define COMPONENTS_PARTICLEEMITTER_H__
#include "Core/Component.h"

namespace dd
{
namespace Components
{

struct ParticleEmitter : public Component
{
    int Amount = 10.f;
    float InitialSpeed = 1.f;
    double SpawnRate = 1.f;
    double TimeSinceLastSpawn = 0;

    //System variables
    float GravityScale = 1.0f;
};

}

}

#endif
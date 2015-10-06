#ifndef COMPONENTS_PARTICLEEMITTER_H__
#define COMPONENTS_PARTICLEEMITTER_H__
#include "Core/Component.h"

namespace dd
{
namespace Components
{

struct ParticleEmitter : public Component
{
    int NumberOfTicks = 10.f;
    float InitialSpeed = 1.f;
    double SpawnRate = 1;
    double TimeSinceLastSpawn = 0;
    int MaxCount = 0;
    float EmittingAngle = 0.f;
    float Spread = 0.f;
    float Speed = 2.f;

    //ParticleSystem variables
    float GravityScale = 1.0f;
};

}

}

#endif
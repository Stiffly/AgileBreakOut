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
    int ParticlesPerTick = 1;
	//TODO: Refactor. Have cooldown instead of this
    double TimeSinceLastSpawn = 100;
    int MaxCount = 0;
    float EmittingAngle = 0.f;
    float Spread = 0.f;
    float Speed = 2.f;
    double LifeTime = 100;

    //System variables
    float GravityScale = 1.0f;
};

}

}

#endif
#ifndef COMPONENTS_PARTICLE_H__
#define COMPONENTS_PARTICLE_H__
#include "Core/Component.h"

namespace dd {

namespace ParticleFlags {

enum Type {
    waterParticle = 0,
    zombieParticle = 1 << 1,
    wallParticle = 1 << 2,
    springParticle = 1 << 3,
    elasticParticle = 1 << 4,
    viscousParticle = 1 << 5,
    powderParticle = 1 << 6,
    tensileParticle = 1 << 7,
    colorMixingParticle = 1 << 8,
    destructionListenerParticle = 1 << 9,
    barrierParticle = 1 << 10,
    staticPressureParticle = 1 << 11,
    reactiveParticle = 1 << 12,
    repulsiveParticle = 1 << 13,
    fixtureContactListenerParticle = 1 << 14,
    particleContactListenerParticle = 1 << 15,
    fixtureContactFilterParticle = 1 << 16,
    particleContactFilterParticle = 1 << 17
};
}

namespace Components
{

struct Particle : public Component
{
    float Radius = 0.5f;
    float LifeTime = 5.0f;

    ParticleFlags::Type Flags =
            ParticleFlags::Type::waterParticle |
            ParticleFlags::Type::zombieParticle |
            ParticleFlags::Type::wallParticle |
            ParticleFlags::Type::springParticle |
            ParticleFlags::Type::elasticParticle |
            ParticleFlags::Type::viscousParticle |
            ParticleFlags::Type::powderParticle |
            ParticleFlags::Type::tensileParticle |
            ParticleFlags::Type::colorMixingParticle |
            ParticleFlags::Type::destructionListenerParticle |
            ParticleFlags::Type::barrierParticle |
            ParticleFlags::Type::staticPressureParticle |
            ParticleFlags::Type::reactiveParticle |
            ParticleFlags::Type::repulsiveParticle |
            ParticleFlags::Type::fixtureContactListenerParticle |
            ParticleFlags::Type::particleContactListenerParticle |
            ParticleFlags::Type::fixtureContactFilterParticle |
            ParticleFlags::Type::particleContactFilterParticle;

};

}

}

#endif
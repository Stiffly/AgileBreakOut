#ifndef COMPONENTS_PARTICLE_H__
#define COMPONENTS_PARTICLE_H__
#include "Core/Component.h"

namespace dd {

namespace ParticleFlags {

enum Type {
    Water = 0,
    Zombie = 1 << 1,
    Wall = 1 << 2,
    Spring = 1 << 3,
    Elastic = 1 << 4,
    Viscous = 1 << 5,
    Powder = 1 << 6,
    Tensile = 1 << 7,
    ColorMixing = 1 << 8,
    DestructionListener = 1 << 9,
    Barrier = 1 << 10,
    StaticPressure = 1 << 11,
    Reactive = 1 << 12,
    Repulsive = 1 << 13,
    FixtureContactListener = 1 << 14,
    ParticleContactListener = 1 << 15,
    FixtureContactFilter = 1 << 16,
    ParticleContactFilter = 1 << 17
};
}

namespace Components
{

struct Particle : public Component
{
	EntityID ParticleSystem;
    float Radius = 0.5f;
	double TimeLived = 0;
	double LifeTime = 5;

	ParticleFlags::Type Flags = static_cast<ParticleFlags::Type>(
			ParticleFlags::Water
			| ParticleFlags::Zombie
			| ParticleFlags::Wall
			| ParticleFlags::Spring
			| ParticleFlags::Elastic
			| ParticleFlags::Viscous
			| ParticleFlags::Powder
			| ParticleFlags::Tensile
			| ParticleFlags::ColorMixing
			| ParticleFlags::DestructionListener
			| ParticleFlags::Barrier
			| ParticleFlags::StaticPressure
			| ParticleFlags::Reactive
			| ParticleFlags::Repulsive
			| ParticleFlags::FixtureContactListener
			| ParticleFlags::ParticleContactListener
			| ParticleFlags::FixtureContactFilter
            | ParticleFlags::ParticleContactFilter
	);
};

}

}

#endif
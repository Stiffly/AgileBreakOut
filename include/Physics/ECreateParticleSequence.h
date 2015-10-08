#ifndef EVENTS_ECREATEPARTICLESEQUENCE_H__
#define EVENTS_ECREATEPARTICLESEQUENCE_H__

#include "Core/EventBroker.h"
#include "Physics/CParticle.h"

namespace dd
{
namespace Events
{

struct CreateParticleSequence : public Event
{
    //Emitter
    double EmitterLifeTime = 100;
    glm::vec3 Position = glm::vec3(0);
    float GravityScale = 0.f;
    float SpawnRate = 1.f;
    int NumberOfTicks = 100;
    float Speed = 1.f;
    int ParticlesPerTick = 1.f;
    float Spread = 1.f;
    float EmittingAngle = 0.f;
    float MaxCount = 0;

    //Particle
    std::string SpriteFile = "Texures/Core/ErrorTexture.png";
    double ParticleLifeTime = 3.f;
    ParticleFlags::Type Flags = static_cast<ParticleFlags::Type>(ParticleFlags::Powder | ParticleFlags::ParticleContactFilter | ParticleFlags::FixtureContactFilter);
    float Radius = 1.f;
};

}
}

#endif


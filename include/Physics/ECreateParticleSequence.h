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
	float RadiusDistribution = 0;
	glm::vec4 Color = glm::vec4(0);
	EntityID parent = 0;
	// values to interpolate between.
	bool DefaultScale = false;
	std::vector<glm::vec3> ScaleValues;
	std::vector<float> AlphaValues;
	std::vector<glm::vec3> VelocityValues;

    //Particle
    std::string SpriteFile = "";
    double ParticleLifeTime = 3.f;
    ParticleFlags::Type Flags = static_cast<ParticleFlags::Type>(ParticleFlags::Powder | ParticleFlags::ParticleContactFilter | ParticleFlags::FixtureContactFilter);

};

}
}

#endif


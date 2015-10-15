#ifndef SYSTEMS_SOUNDSYSTEM_H__
#define SYSTEMS_SOUNDSYSTEM_H__

#include "AL/al.h"
#include "AL/alc.h"

#include "Core/System.h"
#include "Core/World.h"
#include "Core/EventBroker.h"
#include "Core/ResourceManager.h"
#include "Sound.h"
#include "Sound/EPlaySound.h"
#include "Sound/EStopSound.h"
#include "Sound/EMasterVolume.h"
#include "Sound/CCollisionSound.h"
#include "Physics/EContact.h"
#include "Game/CBall.h"
#include "Game/CBrick.h"
#include "Game/CPad.h"
#include "Physics/ECreateParticleSequence.h"

namespace dd
{
namespace Systems
{

class SoundSystem : public System {
public:
    SoundSystem(World *world, std::shared_ptr<dd::EventBroker> eventBroker)
            : System(world, eventBroker) { }

    ~SoundSystem();

    void Initialize() override;
    void Update(double dt) override;

private:
    float m_BGMMasterVolume = 1.f;
    float m_SFXMasterVolume = 1.f;
    std::map<ALuint, Sound*> m_BGMSourcesToBuffers;
    std::map<ALuint, Sound*> m_SFXSourcesToBuffers;
    ALCdevice* m_Device = nullptr;

    //Events
    dd::EventRelay<SoundSystem, dd::Events::PlaySound> m_EPlaySFX;
    bool OnPlaySound(const dd::Events::PlaySound &event);
    //On contact: play the sound given in the CCOllisionSound.
    dd::EventRelay<SoundSystem, dd::Events::Contact> m_EContact;
    bool OnContact(const dd::Events::Contact &event);
    dd::EventRelay<SoundSystem, dd::Events::StopSound> m_EStopSound;
    bool OnStopSound(const dd::Events::StopSound &event);
    dd::EventRelay<SoundSystem, dd::Events::MasterVolume> m_EMasterVolume;
    bool OnMasterVolume(const dd::Events::MasterVolume &event);

    ALuint CreateSource();
};

}
}

#endif

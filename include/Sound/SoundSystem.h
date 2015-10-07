#ifndef DAYDREAM_SOUND_H
#define DAYDREAM_SOUND_H


#include "AL/al.h"
#include "AL/alc.h"
#include "Core/System.h"
#include "Core/World.h"
#include "Core/EventBroker.h"
#include "Sound.h"
#include "Sound/EPlaySound.h"
#include "Sound/EStopSound.h"
#include "Sound/EMasterVolume.h"
#include "Physics/EContact.h"
#include "Game/CBall.h"
#include "Game/CBrick.h"
#include "Game/CPad.h"
#include "Sound/CCollisionSound.h"
#include "Core/ResourceManager.h"

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
    //Events
    dd::EventRelay<SoundSystem, dd::Events::PlaySound> m_EPlaySFX;
    dd::EventRelay<SoundSystem, dd::Events::Contact> m_EContact;
    dd::EventRelay<SoundSystem, dd::Events::StopSound> m_EStopSound;
    dd::EventRelay<SoundSystem, dd::Events::MasterVolume> m_EMasterVolume;
    bool OnPlaySound(const dd::Events::PlaySound &event);
    bool OnContact(const dd::Events::Contact &event);
    bool OnStopSound(const dd::Events::StopSound &event);
    bool OnMasterVolume(const dd::Events::MasterVolume &event);

    ALuint CreateSource();

    std::map<ALuint, Sound*> m_BGMSourcesToBuffers;
    std::map<ALuint, Sound*> m_SFXSourcesToBuffers;

    ALCdevice* m_Device;

    float m_BGMMasterVolume, m_SFXMasterVolume;

};
}
}

#endif //DAYDREAM_SOUND_H

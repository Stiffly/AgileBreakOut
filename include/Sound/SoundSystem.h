#ifndef DAYDREAM_SOUND_H
#define DAYDREAM_SOUND_H


#include "AL/al.h"
#include "AL/alc.h"
#include "Core/System.h"
#include "Core/World.h"
#include "Sound.h"
#include "Sound/EPlaySFX.h"
#include "Physics/EContact.h"
#include "Core/EventBroker.h"
#include "Game/CBall.h"
#include "Game/CBrick.h"
#include "Game/CPad.h"
#include "Sound/CCollisionSound.h"
#include "Core/ResourceManager.h"


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

    //void UpdateEntity(double dt, EntityID entity, EntityID parent) override;
    void Update(double dt) override;
    //void OnComponentCreated(std::string type, std::shared_ptr<Component> component) override;
    //void OnComponentRemoved(std::string type, Component* component) override;
    //void PlaySound(Components::SoundEmitter* emitter, std::string path); // Use if you want to play a temporary .wav file not from component
    //void PlaySound(std::shared_ptr<Components::SoundEmitter> emitter); // Use if you want to play .wav file from component
    //void StopSound(std::shared_ptr<Components::SoundEmitter> emitter);




private:
    //Events
    dd::EventRelay<SoundSystem, dd::Events::PlaySFX> m_EPlaySFX;
    dd::EventRelay<SoundSystem, dd::Events::Contact> m_EContact;
    bool OnPlaySFX(const dd::Events::PlaySFX &event);
    bool OnContact(const dd::Events::Contact &event);

    ALuint LoadFile(std::string fileName);
    ALuint CreateSource();

    //std::map<Component*, ALuint> m_Sources;


    //Temp
    ALuint m_Source;

};
}
}

#endif //DAYDREAM_SOUND_H

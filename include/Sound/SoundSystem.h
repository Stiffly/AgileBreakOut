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
    void Update(double dt) override;




private:
    //Events
    dd::EventRelay<SoundSystem, dd::Events::PlaySFX> m_EPlaySFX;
    dd::EventRelay<SoundSystem, dd::Events::Contact> m_EContact;
    bool OnPlaySFX(const dd::Events::PlaySFX &event);
    bool OnContact(const dd::Events::Contact &event);
    ALuint CreateSource();

    std::map<Component*, ALuint> m_Sources;

    //std::list<ALuint>
    //Temp
    ALuint m_Source;

};
}
}

#endif //DAYDREAM_SOUND_H

#include "PrecompiledHeader.h"
#include "Sound/SoundSystem.h"

dd::Systems::SoundSystem::~SoundSystem()
{
    alSourcei(m_Source, AL_BUFFER, 0);
    alDeleteSources(1, &m_Source);
}

void dd::Systems::SoundSystem::Initialize()
{   //initialize OpenAL
    ALCdevice* device = alcOpenDevice(NULL);
    ALCcontext* context;

    if (device) {
        context = alcCreateContext(device, NULL);
        alcMakeContextCurrent(context);
    }
    else {
        LOG_ERROR("OpenAL failed to initialize.");
    }

    alGetError();
    alSpeedOfSound(340.29f); // Speed of sound m/s
    alDistanceModel(AL_INVERSE_DISTANCE_CLAMPED);

    //Subscribe to events
    EVENT_SUBSCRIBE_MEMBER(m_EContact, &SoundSystem::OnContact);
    EVENT_SUBSCRIBE_MEMBER(m_EPlaySFX, &SoundSystem::OnPlaySFX);
}

void dd::Systems::SoundSystem::Update(double dt)
{
    const ALfloat pos[3] = {0, 0, 0};
    alListenerfv(AL_POSITION, pos);

    alSourcefv(m_Source, AL_POSITION, pos);
}

bool dd::Systems::SoundSystem::OnPlaySFX(const dd::Events::PlaySFX &event)
{
    m_Source = CreateSource();
    Sound *sound = ResourceManager::Load<Sound>(event.path);
    ALuint buffer = sound->Buffer();
    alSourcei(m_Source, AL_BUFFER, buffer);
    alSourcePlay(m_Source);
}

bool dd::Systems::SoundSystem::OnContact(const dd::Events::Contact &event)
{
    //Check which entity has the collisionSound component.
    auto collisionSound = m_World->GetComponent<Components::CollisionSound>(event.Entity1);
    if (collisionSound == NULL) {
        collisionSound = m_World->GetComponent<Components::CollisionSound>(event.Entity2);
        if (collisionSound == NULL) {
            //None of the entities had a collisionSound component.
            return false;
        }
    }

    //Send play-sound event
    dd::Events::PlaySFX e;
    e.path = collisionSound->filePath;
    EventBroker->Publish(e);
    return true;
}

ALuint dd::Systems::SoundSystem::CreateSource()
{
    ALuint source;
    alGenSources((ALuint)1, &source);

    return source;
}

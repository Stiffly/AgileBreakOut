#include "PrecompiledHeader.h"
#include "Sound/SoundSystem.h"

dd::Systems::SoundSystem::~SoundSystem()
{
    alSourcei(m_Source, AL_BUFFER, 0);
    alDeleteSources(1, &m_Source);
    alcCloseDevice(m_Device);
}

void dd::Systems::SoundSystem::Initialize()
{   //initialize OpenAL
    m_Device = alcOpenDevice(NULL);
    ALCcontext* context;

    if (m_Device) {
        context = alcCreateContext(m_Device, NULL);
        alcMakeContextCurrent(context);
    }
    else {
        LOG_ERROR("OpenAL failed to initialize.");
    }

    alGetError();

    //Create sources
    m_Source = CreateSource();
    m_BGMSource = CreateSource();
    m_BGMSupportSource = CreateSource();

    alSpeedOfSound(340.29f); // Speed of sound m/s
    alDistanceModel(AL_INVERSE_DISTANCE_CLAMPED);

    const ALfloat pos[3] = {0, 0, 0};
    alListenerfv(AL_POSITION, pos);
    alSourcefv(m_Source, AL_POSITION, pos);

    //Subscribe to events
    EVENT_SUBSCRIBE_MEMBER(m_EContact, &SoundSystem::OnContact);
    EVENT_SUBSCRIBE_MEMBER(m_EPlaySFX, &SoundSystem::OnPlaySFX);
    EVENT_SUBSCRIBE_MEMBER(m_EPlayBGM, &SoundSystem::OnPlayBGM);

    //Todo: Move this
    dd::Events::PlayBGM e;
    e.path = "Sounds/BGM/soft-guitar.wav";
    e.SupportBGM = false;
    EventBroker->Publish(e);

    dd::Events::PlayBGM e2;
    e2.path = "Sounds/BGM/water-flowing.wav";
    e2.SupportBGM = true;
    e2.volume = 0.1;
    EventBroker->Publish(e2);
}

void dd::Systems::SoundSystem::Update(double dt)
{

}

bool dd::Systems::SoundSystem::OnPlayBGM(const dd::Events::PlayBGM &event)
{
    Sound *sound = ResourceManager::Load<Sound>(event.path);
    ALuint buffer = sound->Buffer();
    ALuint source = CreateSource();

    alSourcei(source, AL_BUFFER, buffer);
    alSourcei(source, AL_LOOPING, AL_TRUE);
    alSourcef(source, AL_GAIN, event.volume);
    alSourcePlay(source);

    m_SourcesToBuffers[source] = buffer;
}

bool dd::Systems::SoundSystem::OnPlaySFX(const dd::Events::PlaySFX &event)
{
    Sound *sound = ResourceManager::Load<Sound>(event.path);
    ALuint buffer = sound->Buffer();
    ALuint source = CreateSource();

    alSourcei(source, AL_BUFFER, buffer);
    alSourcei(source, AL_LOOPING, AL_FALSE);
    alSourcePlay(source);

    m_SourcesToBuffers[source] = buffer;
}

bool dd::Systems::SoundSystem::OnContact(const dd::Events::Contact &event)
{
    //Check which entity has the collisionSound component.
    auto collisionSound = m_World->GetComponent<Components::CollisionSound>(event.Entity1);
    if (collisionSound == nullptr) {
        collisionSound = m_World->GetComponent<Components::CollisionSound>(event.Entity2);
        if (collisionSound == nullptr) {
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

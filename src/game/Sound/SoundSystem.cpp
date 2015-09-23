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

    //Todo: Move this
    {
        dd::Events::PlaySFX e;
        e.path = "Sounds/BGM/soft-guitar.wav";
        e.loop = true;
        EventBroker->Publish(e);
    }
    {
        dd::Events::PlaySFX e;
        e.path = "Sounds/BGM/water-flowing.wav";
        e.volume = 0.3f;
        e.loop = true;
        EventBroker->Publish(e);
    }
}

void dd::Systems::SoundSystem::Update(double dt)
{
    //LOG_INFO("Sources : %i", m_SourcesToBuffers.size());

    std::vector<ALuint> deleteList;
    for (auto item : m_SourcesToBuffers) {
        ALint sourceState;
        alGetSourcei(item.first, AL_SOURCE_STATE, &sourceState);
        if (sourceState == AL_STOPPED) {
            deleteList.push_back(item.first);
        }
    }

    for (int i = 0; i < deleteList.size(); i++) {
        alDeleteSources(1, &deleteList[i]);
        //alDeleteBuffers(1, &m_SourcesToBuffers[deleteList[i]]);
        m_SourcesToBuffers.erase(deleteList[i]);
    }

}

bool dd::Systems::SoundSystem::OnPlaySFX(const dd::Events::PlaySFX &event)
{
    //Loading and binding sound buffer to source
    Sound *sound = ResourceManager::Load<Sound>(event.path);
    ALuint buffer = sound->Buffer();
    ALuint source = CreateSource();
    m_SourcesToBuffers[source] = buffer;
    alSourcei(source, AL_BUFFER, buffer);

    //Sound settings
    alSourcef(source, AL_GAIN, event.volume);
    if (event.loop) {
        alSourcei(source, AL_LOOPING, AL_TRUE);
    }
    else if (!event.loop)
    {
        alSourcei(source, AL_LOOPING, AL_FALSE);
    }


    //Play
    alSourcePlay(source);
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

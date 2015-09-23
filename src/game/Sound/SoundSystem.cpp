#include "PrecompiledHeader.h"
#include "Sound/SoundSystem.h"

dd::Systems::SoundSystem::~SoundSystem()
{
    alcCloseDevice(m_Device);
}

void dd::Systems::SoundSystem::Initialize()
{
    //initialize OpenAL
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

    const ALfloat pos[3] = {0, 0, 0};
    alListenerfv(AL_POSITION, pos);

    //Subscribe to events
    EVENT_SUBSCRIBE_MEMBER(m_EContact, &SoundSystem::OnContact);
    EVENT_SUBSCRIBE_MEMBER(m_EPlaySFX, &SoundSystem::OnPlaySound);
    EVENT_SUBSCRIBE_MEMBER(m_EStopSound, &SoundSystem::OnStopSound);

    //Todo: Move this
    {
        dd::Events::PlaySound e;
        e.path = "Sounds/BGM/soft-guitar.wav";
        e.loop = true;
        EventBroker->Publish(e);
    }
    {
        dd::Events::PlaySound e;
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

ALuint dd::Systems::SoundSystem::CreateSource()
{
    ALuint source;
    alGenSources((ALuint)1, &source);

    return source;
}

bool dd::Systems::SoundSystem::OnPlaySound(const dd::Events::PlaySound &event)
{
    //Loading and binding sound buffer to source
    Sound *sound = ResourceManager::Load<Sound>(event.path);
    if (sound == nullptr) {
       return false;
    }
    ALuint source = CreateSource();
    m_SourcesToBuffers[source] = sound;
    ALuint buffer = sound->Buffer();
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
    return true;
}

bool dd::Systems::SoundSystem::OnStopSound(const dd::Events::StopSound &event)
{
    for (auto item : m_SourcesToBuffers) {
        if (item.second->Path() == event.path) {
            alSourceStop(item.first);
            return true;
        }
    }
    return false;
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

    {
        dd::Events::StopSound e;
        e.path = "Sounds/BGM/soft-guitar.wav";
        EventBroker->Publish(e);
    }
    //Send play-sound event
    dd::Events::PlaySound e;
    e.path = collisionSound->filePath;
    EventBroker->Publish(e);
    return true;
}



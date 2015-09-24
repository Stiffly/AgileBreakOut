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

    //Probably unnecessary. vec3(0) probably default.
    const ALfloat pos[3] = {0, 0, 0};
    //alListenerfv(AL_POSITION, pos);

    m_SFXMasterVolume = 1.f;
    m_BGMMasterVolume = 1.f;

    //Subscribe to events
    EVENT_SUBSCRIBE_MEMBER(m_EContact, &SoundSystem::OnContact);
    EVENT_SUBSCRIBE_MEMBER(m_EPlaySFX, &SoundSystem::OnPlaySound);
    EVENT_SUBSCRIBE_MEMBER(m_EStopSound, &SoundSystem::OnStopSound);
    EVENT_SUBSCRIBE_MEMBER(m_EMasterVolume, &SoundSystem::OnMasterVolume);


    //Todo: Move this
    {
        dd::Events::PlaySound e;
        e.path = "Sounds/BGM/under-the-sea-instrumental.wav";
        e.isAmbient = true;
        EventBroker->Publish(e);
    }
    {
        dd::Events::PlaySound e;
        e.path = "Sounds/BGM/water-flowing.wav";
        e.volume = 0.3f;
        e.isAmbient = true;
        EventBroker->Publish(e);
    }
    {
        dd::Events::MasterVolume e;
        e.isAmbient = true;
        e.gain = 0.001f;
        EventBroker->Publish(e);
    }
}

void dd::Systems::SoundSystem::Update(double dt)
{
    //Clean up none-active sources
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
    float relativeVolume = 1.f;
    if (event.isAmbient) {
        alSourcei(source, AL_LOOPING, AL_TRUE);
        relativeVolume = m_BGMMasterVolume;
    }
    else if (!event.isAmbient)
    {
        alSourcei(source, AL_LOOPING, AL_FALSE);
        relativeVolume = m_SFXMasterVolume;
    }

    alSourcef(source, AL_GAIN, (event.volume * relativeVolume));
    alSourcef(source, AL_PITCH, event.pitch);


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

bool dd::Systems::SoundSystem::OnMasterVolume(const dd::Events::MasterVolume &event)
{
    if (event.isAmbient) {
        m_BGMMasterVolume = event.gain;
    }
    else if (!event.isAmbient) {
        m_SFXMasterVolume = event.gain;
    }
    m_World->GetEntities();
}

//On contact: play the sound given in the CCOllisionSound.
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
    dd::Events::PlaySound e;
    e.path = collisionSound->filePath;
    e.isAmbient = false;
    EventBroker->Publish(e);
    //return true;
}



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

    //Subscribe to events
    EVENT_SUBSCRIBE_MEMBER(m_EContact, &SoundSystem::OnContact);
    EVENT_SUBSCRIBE_MEMBER(m_EPlaySFX, &SoundSystem::OnPlaySound);
    EVENT_SUBSCRIBE_MEMBER(m_EStopSound, &SoundSystem::OnStopSound);
    EVENT_SUBSCRIBE_MEMBER(m_EMasterVolume, &SoundSystem::OnMasterVolume);

    //Todo: Move this
    {
        dd::Events::PlaySound e;
        e.FilePath = "Sounds/BGM/under-the-sea-instrumental.wav";
        e.IsAmbient = true;
        EventBroker->Publish(e);
    }
    {
        dd::Events::PlaySound e;
        e.FilePath = "Sounds/BGM/water-flowing.wav";
        e.Gain = 0.3f;
        e.IsAmbient = true;
        EventBroker->Publish(e);
    }


}

void dd::Systems::SoundSystem::Update(double dt)
{
    //Clean up none-active sources
    //Only used for SFX's. BGM's are handled on stop sound.
    std::vector<ALuint> deleteList;
    for (auto& item : m_SFXSourcesToBuffers) {
        ALint sourceState;
        alGetSourcei(item.first, AL_SOURCE_STATE, &sourceState);
        if (sourceState == AL_STOPPED) {
            deleteList.push_back(item.first);
        }
    }

    for (int i = 0; i < deleteList.size(); i++) {
        alDeleteSources(1, &deleteList[i]);
        //alDeleteBuffers(1, &m_SourcesToBuffers[deleteList[i]]);
        m_SFXSourcesToBuffers.erase(deleteList[i]);
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
    Sound *sound = ResourceManager::Load<Sound>(event.FilePath);
    if (sound == nullptr) {
       return false;
    }
    ALuint source = CreateSource();
    ALuint buffer = sound->Buffer();
    alSourcei(source, AL_BUFFER, buffer);
    sound->SetGain(event.Gain);

    //Sound settings
    float relativeVolume = 1.f;
    if (event.IsAmbient) {
        alSourcei(source, AL_LOOPING, AL_TRUE);
        relativeVolume = m_BGMMasterVolume;
        m_BGMSourcesToBuffers[source] = sound;

    }
    else if (!event.IsAmbient)
    {
        m_SFXSourcesToBuffers[source] = sound;
        alSourcei(source, AL_LOOPING, AL_FALSE);
        relativeVolume = m_SFXMasterVolume;
    }

    alSourcef(source, AL_GAIN, (sound->Gain() * relativeVolume));
    alSourcef(source, AL_PITCH, event.Pitch);

    //Play
    alSourcePlay(source);
    return true;
}

bool dd::Systems::SoundSystem::OnStopSound(const dd::Events::StopSound &event)
{
    ALuint itemToDeleted;
    for (auto& item : m_BGMSourcesToBuffers)
    {
        if (item.second->Path() == event.FilePath) {
            itemToDeleted = item.first;
            break;
        }
    }
    alSourceStop(itemToDeleted);
    alDeleteSources(1, &itemToDeleted);
    m_BGMSourcesToBuffers.erase(itemToDeleted);

    //Should not happen because SFX's should be very short.
    for (auto item : m_SFXSourcesToBuffers)
    {
        if (item.second->Path() == event.FilePath) {
            alSourceStop(item.first);
            return true;
        }
    }
    return false;
}

bool dd::Systems::SoundSystem::OnMasterVolume(const dd::Events::MasterVolume &event)
{
    if (event.IsAmbient) {
        m_BGMMasterVolume = event.Gain;
        for (auto& item : m_BGMSourcesToBuffers)
        {
            alSourcef(item.first, AL_GAIN, (item.second->Gain() * m_BGMMasterVolume));
        }
        return true;
    }
    else if (!event.IsAmbient) {
        m_SFXMasterVolume = event.Gain;
        for (auto& item : m_SFXSourcesToBuffers)
        {
            alSourcef(item.first, AL_GAIN, (item.second->Gain() * m_SFXMasterVolume));
        }
        return true;
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

    //Send play-sound event

    {
        dd::Events::PlaySound e;
        e.FilePath = collisionSound->FilePath;
        e.IsAmbient = false;
        EventBroker->Publish(e);
    }

    return true;
}




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



}

void dd::Systems::SoundSystem::Update(double dt)
{
    //Clean up none-active sources
    //Only used for SFX's. BGM's are handled on stop sound.
    std::vector<ALuint> deleteList;
    for (auto item : m_SFXSourcesToBuffers) {
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
    Sound *sound = ResourceManager::Load<Sound>(event.path);
    if (sound == nullptr) {
       return false;
    }
    ALuint source = CreateSource();
    ALuint buffer = sound->Buffer();
    alSourcei(source, AL_BUFFER, buffer);

    //Sound settings
    float relativeVolume = 1.f;
    if (event.isAmbient) {
        alSourcei(source, AL_LOOPING, AL_TRUE);
        relativeVolume = m_BGMMasterVolume;
        m_BGMSourcesToBuffers[source] = sound;

    }
    else if (!event.isAmbient)
    {
        m_SFXSourcesToBuffers[source] = sound;
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

    //TODO: Delete sources for bgms
    ALuint itemToDelete;
    for (auto item : m_BGMSourcesToBuffers)
    {
        if (item.second->Path() == event.path) {
            itemToDelete = item.first;
            break;
        }
    }
    alSourceStop(itemToDelete);
    alDeleteSources(1, &itemToDelete);
    m_BGMSourcesToBuffers.erase(itemToDelete);

    //Should not because SFX's should be very short.
    for (auto item : m_SFXSourcesToBuffers)
    {
        if (item.second->Path() == event.path) {
            alSourceStop(item.first);
            return true;
        }
    }

    return false;
}

bool dd::Systems::SoundSystem::OnMasterVolume(const dd::Events::MasterVolume &event)
{
    //TODO: Make the volume depend on the value given when stored.
    //TODO: .. now it ONLY uses the master volume
    if (event.isAmbient) {
        m_BGMMasterVolume = event.gain;
        for (auto item : m_BGMSourcesToBuffers)
        {
            alSourcef(item.first, AL_GAIN, event.gain);
        }
    }
    else if (!event.isAmbient) {
        m_SFXMasterVolume = event.gain;
        for (auto item : m_SFXSourcesToBuffers)
        {
            alSourcef(item.first, AL_GAIN, event.gain);
        }
    }
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

    {
        dd::Events::PlaySound e;
        e.path = collisionSound->filePath;
        e.isAmbient = false;
        EventBroker->Publish(e);
    }

    {
        Events::CreateParticleSequence e;
        e.Position = glm::vec3(0.f, 0.f, -10.f);
        e.SpriteFile = "Textures/Background.png";
        e.GravityScale = 0.0f;
        e.SpawnRate = 1.f;
        e.NumberOfTicks = 2;
        e.Speed = 1.f;
        e.ParticlesPerTick = 5;
        e.Spread = glm::pi<float>() * 2;
        e.EmittingAngle = glm::pi<float>();
        e.EmitterLifeTime = 50;
        e.ParticleLifeTime = 3.f;
        EventBroker->Publish(e);
    }
    //return true;
}



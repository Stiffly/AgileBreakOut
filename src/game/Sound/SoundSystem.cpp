#include "PrecompiledHeader.h"
#include "Sound/SoundSystem.h"

dd::Systems::SoundSystem::~SoundSystem()
{
	for (auto &bgm : m_BGMSourcesToBuffers) {
		alSourceStop(bgm.first);
		alDeleteSources(1, &bgm.first);
		//bgm.second->~Sound();
	}
	for (auto &sfx : m_SFXSourcesToBuffers) {
		alSourceStop(sfx.first);
		alDeleteSources(1, &sfx.first);
		//sfx.second->~Sound();
	}
	m_BGMSourcesToBuffers.clear();
	m_SFXSourcesToBuffers.clear();
	alcDestroyContext(m_Context);
    alcCloseDevice(m_Device);
}

void dd::Systems::SoundSystem::Initialize()
{
    //initialize OpenAL
    m_Device = alcOpenDevice(NULL);
    

    if (m_Device) {
		m_Context = alcCreateContext(m_Device, NULL);
		alcMakeContextCurrent(m_Context);
    }
    else {
        LOG_ERROR("OpenAL failed to initialize.");
    }
    //alGetError();

    //Subscribe to events
    EVENT_SUBSCRIBE_MEMBER(m_EContact, &SoundSystem::OnContact);
    EVENT_SUBSCRIBE_MEMBER(m_EPlaySFX, &SoundSystem::OnPlaySound);
    EVENT_SUBSCRIBE_MEMBER(m_EStopSound, &SoundSystem::OnStopSound);
    EVENT_SUBSCRIBE_MEMBER(m_EMasterVolume, &SoundSystem::OnMasterVolume);
	EVENT_SUBSCRIBE_MEMBER(m_EGameStart, &SoundSystem::OnGameStart);
	EVENT_SUBSCRIBE_MEMBER(m_EKrakenAppear, &SoundSystem::OnKrakenAppear);

	m_SFXMasterVolume = ResourceManager::Load<ConfigFile>("Config.ini")->GetValue<float>("Audio.SFXVolume", 1.f);
	m_BGMMasterVolume = ResourceManager::Load<ConfigFile>("Config.ini")->GetValue<float>("Audio.BGMVolume", 1.f);

	//TODO: Move this
	Events::PlaySound e;
	e.FilePath = MENU_BGM;
	e.IsAmbient = true;
	e.Gain = 0.2f;
	EventBroker->Publish(e);
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
			alSourceStop(itemToDeleted);
			alDeleteSources(1, &itemToDeleted);
			m_BGMSourcesToBuffers.erase(itemToDeleted);
			return true;
        }
    }
    

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
		e.Gain = 0.8f;
        EventBroker->Publish(e);
    }

    return true;
}

bool dd::Systems::SoundSystem::OnGameStart(const dd::Events::GameStart &event)
{
	{
		dd::Events::StopSound e;
		e.FilePath = STORY_VOICE;
		EventBroker->Publish(e);
	}
	{
		dd::Events::StopSound e;
		e.FilePath = MENU_BGM;
		EventBroker->Publish(e);
	}
    {
        dd::Events::PlaySound e;
        e.FilePath = GAME_BGM;
        e.IsAmbient = true;
        EventBroker->Publish(e);
    }
    {
        dd::Events::PlaySound e;
        e.FilePath = WATER_BGM;
        e.Gain = 0.3f;
        e.IsAmbient = true;
        EventBroker->Publish(e);
    }
	return true;
}

bool dd::Systems::SoundSystem::OnKrakenAppear(const dd::Events::KrakenAppear &event)
{
	{
		dd::Events::StopSound e;
		e.FilePath = GAME_BGM;
		EventBroker->Publish(e);
	}
	{
		dd::Events::StopSound e;
		e.FilePath = WATER_BGM;
		EventBroker->Publish(e);
	}
	{
		dd::Events::PlaySound e;
		e.FilePath = BOSS_BGM;
		e.IsAmbient = true;
		e.Gain = 0.4f;
		EventBroker->Publish(e);
	}
	{
		dd::Events::PlaySound e;
		e.FilePath = "Sounds/Boss/boss-roar.wav";
		EventBroker->Publish(e);
	}
	return true;
}




/*
	This file is part of Daydream Engine.
	Copyright 2014 Adam Byléhn, Tobias Dahl, Simon Holmberg, Viktor Ljung

	Daydream Engine is free software: you can redistribute it and/or modify
	it under the terms of the GNU Lesser General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	Daydream Engine is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public License
	along with Daydream Engine.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef DAYDREAM_SOUND_H
#define DAYDREAM_SOUND_H


#include "AL/al.h"
#include "AL/alc.h"
#include "Core/System.h"
#include "Core/World.h"
#include "Core/EKeyDown.h"
#include "Sound/EPlaySFX.h"
#include "Physics/EContact.h"
#include "Core/EventBroker.h"
#include "Game/CBall.h"
#include "Game/CBrick.h"
#include "Game/CPad.h"
#include "Sound/CCollisionSound.h"


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
    dd::EventRelay<SoundSystem, dd::Events::KeyDown> m_EKeyDown;
    dd::EventRelay<SoundSystem, dd::Events::PlaySFX> m_EPlaySFX;
    dd::EventRelay<SoundSystem, dd::Events::Contact> m_EContact;

    bool OnKeyDown(const dd::Events::KeyDown &event);
    bool OnPlaySFX(const dd::Events::PlaySFX &event);
    bool OnContact(const dd::Events::Contact &event);

    ALuint LoadFile(std::string fileName);
    ALuint CreateSource();

    //File-info
    char m_Type[4];
    unsigned long m_Size, m_ChunkSize;
    short m_FormatType, m_Channels;
    unsigned long m_SampleRate, m_AvgBytesPerSec;
    short m_BytesPerSample, m_BitsPerSample;
    unsigned long m_DataSize;


    //std::map<Component*, ALuint> m_Sources;
    std::map<std::string, ALuint> m_BufferCache;


    //Temp
    ALuint m_Source;

};
}
}

#endif //DAYDREAM_SOUND_H

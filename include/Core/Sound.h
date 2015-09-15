//
// Created by Adam on 2015-09-09.
//

#ifndef DAYDREAM_SOUND_H
#define DAYDREAM_SOUND_H

#include <fstream>

#include "AL/al.h"
#include "AL/alc.h"
#include "Core/System.h"
#include "Core/World.h"
#include "Core/EKeyDown.h"
#include "Core/EventBroker.h"

namespace dd
{
namespace Systems
{
class Sound : public System {
public:
    Sound(World *world, std::shared_ptr<dd::EventBroker> eventBroker)
            : System(world, eventBroker) { }
    ~Sound();
    void Initialize() override;

    //void UpdateEntity(double dt, EntityID entity, EntityID parent) override;
    void Update(double dt) override;
    //void OnComponentCreated(std::string type, std::shared_ptr<Component> component) override;
    //void OnComponentRemoved(std::string type, Component* component) override;
    //void PlaySound(Components::SoundEmitter* emitter, std::string path); // Use if you want to play a temporary .wav file not from component
    //void PlaySound(std::shared_ptr<Components::SoundEmitter> emitter); // Use if you want to play .wav file from component // imon no hate plx T.T
    //void StopSound(std::shared_ptr<Components::SoundEmitter> emitter);




private:
    dd::EventRelay<Sound, dd::Events::KeyDown> m_EKeyDown;
    bool OnKeyDown(const dd::Events::KeyDown &event);

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

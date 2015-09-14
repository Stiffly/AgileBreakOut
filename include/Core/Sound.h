//
// Created by Adam on 2015-09-09.
//

#ifndef DAYDREAM_SOUND_H
#define DAYDREAM_SOUND_H

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


    //void UpdateEntity(double dt, EntityID entity, EntityID parent) override;
    void Update(double dt) override;
    //void OnComponentCreated(std::string type, std::shared_ptr<Component> component) override;
    //void OnComponentRemoved(std::string type, Component* component) override;
    //void PlaySound(Components::SoundEmitter* emitter, std::string path); // Use if you want to play a temporary .wav file not from component
    //void PlaySound(std::shared_ptr<Components::SoundEmitter> emitter); // Use if you want to play .wav file from component // imon no hate plx T.T
    //void StopSound(std::shared_ptr<Components::SoundEmitter> emitter);


    void Initialize() override;

private:
    dd::EventRelay<Sound, dd::Events::KeyDown> m_EKeyDown;

    ALuint LoadFile(std::string fileName);
    ALuint CreateSource();

    //File-info
    char m_type[4];
    unsigned long m_size, m_chunkSize;
    short m_formatType, m_channels;
    unsigned long m_sampleRate, m_avgBytesPerSec;
    short m_bytesPerSample, m_bitsPerSample;
    unsigned long m_dataSize;


    std::map<Component*, ALuint> m_Sources;
    std::map<std::string, ALuint> m_BufferCache;

    bool OnKeyDown(const dd::Events::KeyDown &event);

    //Temp
    ALuint m_source;

};
}
}

#endif //DAYDREAM_SOUND_H
